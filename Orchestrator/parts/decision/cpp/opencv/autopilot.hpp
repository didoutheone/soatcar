#ifndef SOATDECISIONPART_H
#define SOATDECISIONPART_H

#include "opencv2/core.hpp"			// opencv includes
#include "opencv2/imgproc.hpp"		// opencv includes
#include "opencv2/highgui.hpp"		// opencv includes
#include "opencv2/videoio.hpp"		// opencv includes

#include "SoatcarState.hpp"
#include "config.hpp"

using namespace cv;
using namespace std;

class AutoPilotPart
{
	public:
		AutoPilotPart();
		void run();
		
	private:
		SoatcarState state{"/var/tmp/soatcarmmf.tmp"};
		AutoPilotConfig config{"autopilot.conf"};
		u64 lastRawImageNo;
		u64 nbInput;
		
		int takeADecision();
		void histogram(Mat & image, Rect rect, int & firstMaxLoc, int & secondMaxLoc, int & firstMaxVal, int & secondMaxVal);
		DataBuf matToDataBuf(Mat image);
		Mat dataBufToMat(DataBuf imageData);
};

#endif