#include <iostream>
#include <string>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "MStringTool.hpp"
#include "MPathTool.hpp"
using namespace std;
using namespace cv;


int main() {
	/* get list of video clips */
	cout << "## get file list of avi";
	vector<string> lClips = toolpath::getAllFiles("data", "*.avi");
	toolstring::printStringList(lClips);

	/* open 1st clip and sample first 10 frames, with 100fr/sample */
	VideoCapture* pClip = new VideoCapture;
	bool isOk = pClip->open(lClips[0].c_str());
	if (!isOk) cout << "clip " << lClips[0].c_str() << " is invalid.\n";
	else {
		Mat imgFr;
		int iFr = 0;
		string fPrefix = "data/fr-";
		string fOut;
		char buffTmp[64];
		while (pClip->read(imgFr) && iFr <= 1000) {
			iFr++;
			if (iFr % 100 == 0) {
				sprintf(buffTmp, "%s%d.png", fPrefix.c_str(), iFr);
				fOut = buffTmp;
				imwrite(fOut, imgFr);
			}
		}
	} pClip->release();
	cout << "Program stop.\n";
	return 0;
}