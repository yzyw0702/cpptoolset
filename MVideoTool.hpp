//===========================
//====>  MVideoTool
//===========================
#ifndef _MVIDEOTOOL_HPP_
#define _MVIDEOTOOL_HPP_

namespace toolvideo {
	#include <iostream>
	#include <string>
	#include <opencv2\core\core.hpp>
	#include <opencv2\highgui\highgui.hpp>
	#include <opencv2\imgproc\imgproc.hpp>
	using namespace std;
	using namespace cv;
	#include "MPathTool.hpp"
	#include "MStringTool.hpp"
	const int mod_video_cps = 1;
	
	class MVideoTool {
	public:                                // interfaces
		/* constructor */
		MVideoTool() {}
		MVideoTool() {
		
		
		
		}

		/* destructor */
		~MVideoTool() {}

	private:                               // internal operations
		
		
		
	private:                               // members
		string data_rootPath; // root path of video clips, configurations etc.
		vector<string> _VNameClips; // name list of clips
		vector<int> _VLenClips;     // length list of clips
		int _iLastClip;        // index of clip traced last time
		int _iLastFrIC;          // index of frame traced last time (in last clip)
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
}


#endif
