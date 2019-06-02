#include <iostream>			// for cout, cerr
#include <thread>       	// for this_thread

#include "autopilot.hpp"
#include "SoatcarState.hpp"
#include "Utils.hpp"

using namespace cv;
using namespace std;

AutoPilotPart::AutoPilotPart()
{
	nbInput = 0;
	lastRawImageNo = 0;
	
	cout << "Initializing Auto Pilot Part..." << endl;

	// Currently nothing to be done
	
    cout << "Auto Pilot part initialized successfully" << endl;
	
	state.setAutoPilotReady(true);
}

void AutoPilotPart::run()
{
	while(!state.getStopFlag())
	{
		clock_type::time_point begin = Utils::getBeginChrono();
		
		u64 rawImgNo = state.getRawImageNo();
		if(rawImgNo != lastRawImageNo)
		{
			takeADecision();
			lastRawImageNo = rawImgNo;
		}
		else
		{
			this_thread::sleep_for(chrono::milliseconds(5));
		}
		
		int freq = Utils::getFrequency(begin);
		state.setAutoPilotLoopRate(freq);
	}
	
	state.setAutoPilotReady(false);
}



int AutoPilotPart::takeADecision()
{
	// Get the image data
	DataBuf imageData = state.getRawImage();

	if(imageData.length == 0) return 0;
	
	Mat image = dataBufToMat(imageData);
	if(image.data == NULL)
	{
		free(imageData.buffer);
		return -1;
	}
	free(imageData.buffer);

	// Test if it was opened successfully
    if(image.empty())
    {
		cout << "Image is empty" << endl;
		return -2;
    }

	// Passage en noir et blanc
	cvtColor(image, image, COLOR_BGR2GRAY);

	// derivée Sobel
	Mat grad_x;
    Mat abs_grad_x;
    Sobel(image, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
	image = abs_grad_x;

	// Take histo at XConfig%
	int i = config.takeHistoAtHeightPct;
	int rectTop = (int)(image.rows * i / 100);
    int rectHeight = (int)(image.rows * 20 / 100);
    int firstMaxLoc = 0, secondMaxLoc = image.cols;
    int firstMaxVal = 0, secondMaxVal = 0;
	histogram(image, Rect(0, rectTop, image.cols, rectHeight), firstMaxLoc, secondMaxLoc, firstMaxVal, secondMaxVal);
	int medLoc = firstMaxLoc + (secondMaxLoc - firstMaxLoc)/2;
	int diffToCenter = medLoc - image.cols/2;

	// increase this diff value to increase the turn
	double myDiffToCenter = diffToCenter * 2;
	if(myDiffToCenter < -image.cols/2) myDiffToCenter = -image.cols/2;
	if(myDiffToCenter > image.cols/2) myDiffToCenter = image.cols/2;
	
	// Set Steering value in state
	int stateValue = (int)state.translate((double)-image.cols/(double)2, (double)image.cols/(double)2, (double)MIN_STATE_FIELDS_VALUE, (double)MAX_STATE_FIELDS_VALUE, myDiffToCenter, false);
	state.setAutoPilotSteeringValue(stateValue);
			
	// Write image in state
	DataBuf data = matToDataBuf(image);
	state.setTreatedImage(data);
	delete data.buffer;
	state.setTreatedImageNo(++nbInput);
	
    return 0;
}


void AutoPilotPart::histogram(Mat & image, Rect rect, int & firstMaxLoc, int & secondMaxLoc, int & firstMaxVal, int & secondMaxVal)
{
	// Histogram
    Mat col_sum;
    reduce(image(rect), col_sum, 0, REDUCE_SUM, CV_32S);
    // find peaks of left and right halves
    int midpoint = int(col_sum.cols/2);
    int val;
	firstMaxVal = secondMaxVal = 0;
    for(int i = 0; i < midpoint; i++)
    {
		double coeff = (((double)i*100.0/(double)col_sum.cols)+5.0)*(105.0-((double)i*100.0/(double)col_sum.cols))/(55.0*55.0);
        val = col_sum.at<int>(i) * coeff;
        if(val > firstMaxVal && val > 500)
        {
            firstMaxLoc = i;
            firstMaxVal = val;
        }
    }
    for(int i = midpoint+1; i < col_sum.cols; i++)
    {
		double coeff = (((double)i*100.0/(double)col_sum.cols)+5.0)*(105.0-((double)i*100.0/(double)col_sum.cols))/(55.0*55.0);
        val = col_sum.at<int>(i) * coeff;
        if(val > secondMaxVal && val > 500)
        {
            secondMaxLoc = i;
            secondMaxVal = val;
        }
    }
}


DataBuf AutoPilotPart::matToDataBuf(Mat image)
{
   int size = image.total() * image.elemSize();
   u8* bytes = new u8[size];  // you will have to delete[] that later
   std::memcpy(bytes, image.data, size * sizeof(u8));
   DataBuf data;
   data.length = size;
   data.buffer = bytes;
   return data;
}


Mat AutoPilotPart::dataBufToMat(DataBuf imageData)
{
	// Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
    Mat rawData(1, imageData.length, CV_8UC1, (void*)imageData.buffer);
    Mat image = imdecode(rawData, IMREAD_COLOR);
	return image;
}