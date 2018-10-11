//===========================
//====>  MVideoTool
//===========================
#ifndef _MVIDEOTOOL_HPP_
#define _MVIDEOTOOL_HPP_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "MSystemTool.hpp"
using namespace std;
using namespace cv;

namespace toolvideo {
	#include "MPathTool.hpp"
	#include "MStringTool.hpp"
	const int mod_video_cps = 1;
	const int mod_dynamic_length = 200;


	class MVideoSrc {
		public: // constructor
			MVideoSrc() {
				init_default();
				initRootPath(".");
			}

			MVideoSrc(const string rootPath, int cps) {
				init_default();
				initRootPath(rootPath.c_str());
				init_with(cps);
			}
			
			MVideoSrc(const string rootPath, const string fVideo, int cps) {
				init_default();
				initRootPath(rootPath);
				init_with(fVideo, cps);
			}

			/* destructor */
			~MVideoSrc() {}

		private: // internal operations
				 // default
			void init_default() {
				data_rootPath = ".\\";

				_iLastClip = 0;
				_iLastFrIC = 0;
				_isStartFromDenovo = false;
				_isStartFromLatest = false;

				_iFrIC = 1;
				_cFrIC = 0;
				_cTtFr = 0;
				_cClips = 0;
				_cps = 1;
				_pClip = new VideoCapture;
			}

			// initialization
			/// <summary>
			/// init data manager
			/// </summary>
			/// <param name="pManData"> data manager pointer </param>
			/// <return> false=failed, true=success </return>
			bool init_with(int cps = 1, int isContinueFromLast = 0) {
				_cTtFr = 0;
				string fListOfClips = data_rootPath + string("list.txt");
				appendVideoSeries(fListOfClips); // load video series
				_pClip = new VideoCapture; // get initial video
				if (isContinueFromLast) initFromLatest();
				else initFromDeNovo();
				_cClips = (int)_VNameClips.size();//get video info
				_cFrIC = (int)_pClip->get(CV_CAP_PROP_FRAME_COUNT);
				_fps = (int)_pClip->get(CV_CAP_PROP_FPS);
				if (isContinueFromLast)  _iClip = _iLastClip;
				else _iClip = _iLastClip = 0;
				if (isContinueFromLast) {
					_iLastFrIC -= (_iLastFrIC % _fps); // adjust to nearest keyframe
					_iFrIC = max(_iLastFrIC - mod_dynamic_length, 0); // reserve a number of frames to compute bg
				}
				else {
					_iFrIC = _iLastFrIC = 1;
				}
				_cps = cps; // get cps and jump length
				_jump = _fps / _cps - 1;
				if (!_pClip->set(CV_CAP_PROP_POS_FRAMES, _iFrIC)) { // set frame pointer to required position
					cout << "video pointer failed to reset to " << _iFrIC << endl;
				} return true;
			}

			bool init_with(const string fVideo, int cps, int isContinueFromLast = 0){
				if (!toolpath::isFileExist(fVideo) || !toolpath::isFileOrDirectory(fVideo)) {
					cout << fVideo.c_str() << " is invalid.\n";
					return false;
				}
				appendClip(fVideo);
				_pClip = new VideoCapture; // get initial video
				if (isContinueFromLast) initFromLatest();
				else initFromDeNovo();
				_cClips = (int)_VNameClips.size();//get video info
				_cFrIC = (int)_pClip->get(CV_CAP_PROP_FRAME_COUNT);
				_fps = (int)_pClip->get(CV_CAP_PROP_FPS);
				if (isContinueFromLast) _iClip = _iLastClip;
				else _iClip = _iLastClip = 0;
				if (isContinueFromLast) {
					_iLastFrIC -= (_iLastFrIC % _fps); // adjust to nearest keyframe
					_iFrIC = max(_iLastFrIC - mod_dynamic_length, 0); // reserve a number of frames to compute bg
				}
				else {
					_iFrIC = _iLastFrIC = 1;
				}
				_cps = cps; // get cps and jump length
				_jump = _fps / _cps - 1;
				if (!_pClip->set(CV_CAP_PROP_POS_FRAMES, _iFrIC)) { // set frame pointer to required position
					cout << "video pointer failed to reset to " << _iFrIC << endl;
				} return true;
			}
			
		public: // interfaces
				/* clip operations */
			bool appendClip(const string fNameClip) {
				string pathClips = toolpath::joinPath(data_rootPath, fNameClip);
				if (toolpath::isFileExist(pathClips)) { //check if exist
														// register this clip
					_VNameClips.push_back(pathClips);
					// collect clip info
					VideoCapture cap(pathClips);
					int currLen = (int)cap.get(CV_CAP_PROP_FRAME_COUNT);
					_VLenClips.push_back(currLen);
					// refresh total frame number
					_cTtFr += currLen;
					cap.release();
					return true;
				}
				else {
					cout << pathClips << ": cannot be accessed.\n";
					return false;
				}
			}

			//bind videos by selection
			bool appendVideoSeries(const string fNameClips = string()) {
				//ask list of input video
				string fIdxVid;
				int cOkLoaded = 0, cNotLoaded = 0;
				if (toolpath::isFileExist(fNameClips)) { // load clips indicated if given list
					fIdxVid = fNameClips;
					// establish an input file stream
					fstream hIdxVid(fIdxVid.c_str(), ios::in);
					//scan list
					string nameVid;
					while (getline(hIdxVid, nameVid)) {
						if (this->appendClip(nameVid)) cOkLoaded++;
						else cNotLoaded++;
					}
					hIdxVid.close();
				}
				else { // by default: load all clips in data_rootPath
					fIdxVid = toolpath::joinPath(this->data_rootPath, "list.txt");
					fstream hIdxVid(fIdxVid.c_str(), ios::out);
					vector<string> lFClips = toolpath::getFiles(this->data_rootPath, "*.avi");
					if (lFClips.size() == 0) {
						cout << "\tNo valid avi clips in " << this->data_rootPath.c_str() << endl;
						hIdxVid.close();
						return false;
					}
					for (int i = 0; i<(int)lFClips.size(); i++) {
						if (this->appendClip(lFClips[i])) {
							cOkLoaded++;
							hIdxVid << lFClips[i] << endl;
						}
						else cNotLoaded++;
					}
					hIdxVid.close();
				}
				if (cNotLoaded > 0)
					cout << "\t" << cOkLoaded << " files loaded; " << cNotLoaded << " files failed.\n";
				return true;
			}

			/* init operations */

			//start a new trace
			void initFromDeNovo() {
				this->_pClip->open(this->_VNameClips[0]);
			}

			//continue trace from last time
			void initFromLatest() {
				//set video
				string pathProg = toolpath::joinPath(this->data_rootPath, "traceRange.txt");
				if (!toolpath::isFileExist(pathProg)) {
					this->initFromDeNovo();
				}
				else {
					this->_pClip->open(this->_VNameClips[this->_iLastClip]);
				}
			}

			/* set operations */
			void initRootPath(const string rootPath) {
				this->data_rootPath = rootPath;
			}

			//reset frame pointer
			//## necessary before start tracking
			bool resetTimer(int iClip = -1, int iFrInClip = -1) {
				// set clip idx
				if (iClip >= 0) this->_iClip = iClip;
				this->_pClip->release();
				this->_pClip->open(this->_VNameClips[this->_iClip]);
				this->_cFrIC = this->_VLenClips[this->_iClip];
				// set frame idx
				if (iFrInClip > 0) this->_iFrIC = iFrInClip;
				else this->_iFrIC = 0;
				return _pClip->set(CV_CAP_PROP_POS_FRAMES, this->_iFrIC);
			}

			bool resetContinuousTimer(int iFr) {
				int cFr = 0;
				for (this->_iClip = 0; cFr <= iFr; this->_iClip++) {
					cFr += this->_VLenClips[this->_iClip];
				}
				this->_iClip--; // go back to current clip
				cFr -= this->_VLenClips[this->_iClip]; // remove extra clip length
				this->_iFrIC = iFr - cFr; // locate exact frame idx in current clip
				this->_pClip->release(); // reopen clip pointer at current clip current frame
				this->_pClip->open(this->_VNameClips[this->_iClip]);
				return this->_pClip->set(CV_CAP_PROP_POS_FRAMES, this->_iFrIC);
			}

			//switch to next vieo if required
			//## true: next video is available
			//## false: the end of all videos
			bool setNextVideo() {
				//close current video
				this->_pClip->release();
				this->_iClip++;
				//check and set to the next
				if (this->_iClip < this->_cClips) { //go to next video
					this->_iLastClip = this->_iClip;
					this->_pClip->open(this->_VNameClips[this->_iClip]);
					//refresh In-Clip properties
					this->_cFrIC = (int) this->_pClip->get(CV_CAP_PROP_FRAME_COUNT);
					this->_iFrIC = 0;
					return true;
				}
				else return false;
			}

			/* get operations */
			//// write stream
			//void writeData(MDataManager* pManData) {
			//	char data[64];
			//	sprintf(data,"%d\t%d\n", this->_iClip, this->_iFrIC);
			//	(*pManData->vid_pIofsTimes) << data;
			//}

			//// export configuration to data manager
			//void exportConfig(MDataManager* pManData) {
			//	FileStorage* pFsConfig = new FileStorage(pManData->data_fConfig, FileStorage::APPEND);
			//	if(!pFsConfig->isOpened()) { // avoid invalid storage
			//		cout << "Export video configuration failed!\n";
			//		return;
			//	}
			//	(*pFsConfig) << "vid_iClipOfLast" << this->_iLastClip;
			//	(*pFsConfig) << "vid_iFrICOfLast" << this->_iLastFrIC;
			//	(*pFsConfig) << "vid_VNameOfClips" << this->_VNameClips;
			//	(*pFsConfig) << "vid_VLenOfClips" << this->_VLenClips;
			//	(*pFsConfig) << "vid_callPerSeconds" << this->_cps;
			//	// instantly save it
			//	pFsConfig->release();
			//}

		//get cps-based frame, change video if necessary
		//## true: frame acquired;
		//## false: reach the video end
		bool readPerCall(Mat& fr) {
			//get frame if available
			this->_pClip->read(fr);
			//get video pointer position
			for (int i = 0; i < this->_jump; i++) {
				if (this->_iFrIC + i > this->_cFrIC - this->_jump + 1) {
					break;
				} this->_pClip->grab();
			}
			//refresh time
			this->_iFrIC += 1 + this->_jump;
			if (_iFrIC >= this->_cFrIC - this->_jump + 1) {
				if (!this->setNextVideo()) {
					return false;
				}
				cout << "switch to clip: " << this->_VNameClips[this->_iClip] << endl;
			} return true;
		}

		void setCps(int cps) {
			this->_cps = cps;
			this->_jump = this->_fps / this->_cps - 1;
		}

		void setJump(int jump) {
			this->_jump = jump;
			this->_cps = this->_fps / (this->_jump + 1);
		}

		string getRootPath() {
			return this->data_rootPath;
		}

		//get call per second
		int getCps() {
			return this->_cps;
		}

		//get frame per second
		int getFps() {
			return this->_fps;
		}

		// get total frame number
		int getLength() {
			return this->_cTtFr;
		}

		//get video pointer
		VideoCapture* getClipPtr() {
			return this->_pClip;
		}

		//get indices of Video and frame
		void getTime(int& iVid, int& iFr) {
			iVid = this->_iClip;
			iFr = this->_iFrIC;
		}



	private: // members
		string data_rootPath;       // root path of video clips, configurations etc.
		vector<string> _VNameClips; // name list of clips
		vector<int> _VLenClips;     // length list of clips
		int _iLastClip;             // index of clip traced last time
		int _iLastFrIC;             // index of frame traced last time (in last clip)
		bool _isStartFromDenovo;    // switch on when start from de novo
		bool _isStartFromLatest;    // switch on when start from latest

		cv::VideoCapture* _pClip;   // video pointer
		int _iFrIC;                 // frame index within current clip
		int _iClip;                 // clip index
		int _cFrIC;                 // frame number of current clip
		int _cTtFr;                 // total frame number of clips
		int _cClips;                // clip number
		int _fps;                   // frame per second
		int _cps;                   // call per second
		int _jump;                  // frames jump per call

	};

	MVideoSrc* createFromSingleVideo(const string fVideo, int cps) {
		if (!toolpath::isFileExist(fVideo)) {
			cout << "\tNo such video: " << fVideo.c_str() <<endl;
			return NULL;
		}
		string rootpath = toolpath::getParentDir(fVideo);
		MVideoSrc* pVid = new MVideoSrc(rootpath, cps);
		return pVid;
	}
	
	MVideoSrc* createFromVideoSeries(const string rootpath, int cps) {
		MVideoSrc* pVid = new MVideoSrc(rootpath, cps);
		return pVid;
	}

}


namespace debug_toolvideo {
	void debug_video() {
		using namespace toolvideo;
		using namespace toolsystem;
		/* test func createFromVideoSeries */
		cout << "##Test func createFromVideoSeries\n";
		string rootpath = "data";
		int cps = 1;
		MVideoSrc* pVid = createFromVideoSeries(rootpath, cps);
		Mat imgFr;
		string fPrefix = "testvideo-";
		string pathOutPng;
		char buffTmp[128];
		int iFr = 0;
		while (pVid->readPerCall(imgFr)) {
			sprintf(buffTmp, "%s%d.png", fPrefix.c_str(), ++iFr);
			pathOutPng = toolpath::joinPath(rootpath, buffTmp);
			if(!imwrite(pathOutPng.c_str(), imgFr))
				cout << "\tFailed to write image " << pathOutPng.c_str() << endl;
			else
				cout << "\tSaved png file " << pathOutPng.c_str() << endl;
			if (iFr >= 10) break;
		}
		
		/* test func createFromSingleVideo */
		cout << "##Test func createFromSingleVideo\n";
		MVideoSrc* pOneVid = createFromSingleVideo(toolpath::joinPath(rootpath, "001-2018-09-26-10.00.00-12.00.00.avi"), 1);
		Mat imgGr;
		Mat imgGr32;
		Mat imgAvg32;
		Mat imgAvg;
		MSystemInfo sys;
		string pathfBg = toolpath::joinPath(rootpath, "bg.png");
		iFr = 0;
		while (pOneVid->readPerCall(imgFr)) {
			if(iFr == 0) {
				imgAvg32 = Mat::zeros(imgFr.size(), CV_32FC1);
			}
			cvtColor(imgFr, imgGr, CV_BGR2GRAY);
			imgGr.convertTo(imgGr32, CV_32F);
			imgAvg32 += imgGr32;
			iFr++;
			if (iFr % 20 == 0) {
				float ratioCpu = sys.getCpuUseRatio();
				float ratioRam = sys.getRamUseRatio();
				cout << "\tAccumulating frame-#" << iFr << " to the background; CPU use-ratio = " << ratioCpu * 100 << "%; RAM use-ratio = " << ratioRam * 100 << "%\n";
			}
			if (iFr >= 400) break;
		} imgAvg32 /= iFr;
		imgAvg32.convertTo(imgAvg, CV_8UC3);
		cvtColor(imgAvg, imgAvg, CV_GRAY2BGR);
		rectangle(imgAvg, Rect(100, 100, 20, 20), Scalar(0, 0, 255), 1);
		if(!imwrite(pathfBg.c_str(), imgAvg))
			cout << "\tFailed to save background image " << pathfBg.c_str() << endl;
		else
			cout << "\tSaved background image " << pathfBg.c_str() << endl;
	}
}


#endif // end of _MVIDEOTOOL_HPP_