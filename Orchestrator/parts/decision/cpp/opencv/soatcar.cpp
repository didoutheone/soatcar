#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "SoatcarState.hpp"

/*
byte * matToBytes(Mat image)
{
   int size = image.total() * image.elemSize();
   byte * bytes = new byte[size];  // you will have to delete[] that later
   std::memcpy(bytes,image.data,size * sizeof(byte));
}

Mat bytesToMat(byte * bytes, int width, int height)
{
    Mat image = Mat(height,width,CV_8UC3,bytes).clone(); // make a copy
    return image;
}

Mat DataBufToMat(DataBuf imageData)
{
	// Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
    Mat rawData(1, imageData.length, CV_8UC1, (void*)imageData.buffer);
    Mat image  =  imdecode(rawData, IMREAD_COLOR);
	return image;
}
*/

using namespace cv;
using namespace std;

// to measure real time, use the high resolution clock if it is steady, the steady clock overwise
using clock_type = std::conditional< std::chrono::high_resolution_clock::is_steady,
                                     std::chrono::high_resolution_clock,
                                     std::chrono::steady_clock >::type;

SoatcarState state("/var/tmp/soatcarmmf.tmp");
void perspective(Mat & input, Mat & output);
void histogram(Mat & image, Rect rect, int & firstMaxLoc, int & secondMaxLoc, int & firstMaxVal, int & secondMaxVal);
int opencvTreat();


int main(int argc, char** argv)
{
	u64 lastImageNo = 0;
	u64 imageNo = 0;
	clock_type::time_point begin;
	int elapsed_msecs;

	// Loop till the end
	while(state.GetStopFlag() == 0)
	{
		begin = clock_type::now();
		imageNo = state.GetImageNo();
		if(imageNo != lastImageNo)
		{
			opencvTreat();
			lastImageNo=imageNo;
			elapsed_msecs = std::chrono::duration_cast<std::chrono::milliseconds>(clock_type::now() - begin).count();
			cout << "Looped in " << elapsed_msecs << "ms" << endl;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	cout << "StopFlag found: exiting" << endl;
}

int opencvTreat()
{
	// Get the image data
	DataBuf imageData = state.GetImage();

	// Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
	Mat rawData(1, imageData.length, CV_8UC1, (void*)imageData.buffer);
	Mat image  =  imdecode(rawData, IMREAD_COLOR);
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

	// Threshold ?
	// threshold(img,img,200,255,CV_THRESH_BINARY); // to delete some noise

	// derivée Sobel
	Mat grad_x;
    Mat abs_grad_x;
    Sobel(image, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
	//addWeighted(abs_grad_x, 0.5, image, 0.5, 0, image);
	image = abs_grad_x;

	// perspective
	//perspective(image, image);
	//elapsed_msecs = (int)(double(clock() - step) / CLOCKS_PER_SEC * 1000);
    //step = clock();
    //cout << "Perspective applied in " << elapsed_msecs << "ms" << endl;


	// Take histo at 40%
	int i = 40;
	int rectTop = (int)(image.rows * i / 100);
    int rectHeight = (int)(image.rows * 20 / 100);
    int firstMaxLoc = 0, secondMaxLoc = image.cols;
    int firstMaxVal = 0, secondMaxVal = 0;
	histogram(image, Rect(0, rectTop, image.cols, rectHeight), firstMaxLoc, secondMaxLoc, firstMaxVal, secondMaxVal);
	int medLoc = firstMaxLoc + (secondMaxLoc - firstMaxLoc)/2;
	int percent = image.cols/100;
	int diffToCenter = medLoc - image.cols/2;
	int sign = 1;
	if(diffToCenter < 0)
	{
		diffToCenter = -diffToCenter;
		sign = -1;
	}

	if(diffToCenter < 5*percent)
	{
		cout << "Medium location " << medLoc << " compared to " << image.cols/2 << ": tout droit." << endl;
	}
	else if(diffToCenter < 15*percent)
	{
		if(sign == 1)
		{
			cout << "Medium location " << medLoc << " compared to " << image.cols/2 << ": légèrement à droite." << endl;
		}
		else
		{
			cout << "Medium location " << medLoc << " compared to " << image.cols/2 << ": légèrement à gauche." << endl;
		}
	}
	else
    {
        if(sign == 1)
        {
            cout << "Medium location " << medLoc << " compared to " << image.cols/2 << ": à droite !" << endl;
        }
        else
        {
            cout << "Medium location " << medLoc << " compared to " << image.cols/2 << ": à gauche !" << endl;
        }
    }

	/*
	// Full Histogram
	Mat mask(Size(image.cols, image.rows), image.type(), Scalar(0));
	for(int i = 0; i < 100; i+=20)
	{
		int rectTop = (int)(image.rows * i / 100);
		int rectHeight = (int)(image.rows * 20 / 100);
		int firstMaxLoc = 0, secondMaxLoc = image.cols;
	    int firstMaxVal = 0, secondMaxVal = 0;
		cout << "Rectangle : (0, " << rectTop << ", width:" << image.cols << ", height:" << rectHeight << ")" << endl;

		histogram(image, Rect(0, rectTop, image.cols, rectHeight), firstMaxLoc, secondMaxLoc, firstMaxVal, secondMaxVal);
		
		line(mask, Point(firstMaxLoc, rectTop), Point(firstMaxLoc, rectTop + rectHeight), CV_RGB(255, 255, 255), 4, 8);
	    line(mask, Point(secondMaxLoc, rectTop), Point(secondMaxLoc, rectTop + rectHeight), CV_RGB(255, 255, 255), 4, 8);
		putText(mask,to_string(firstMaxLoc),Point(firstMaxLoc+5,rectTop+rectHeight/2), FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),1,LINE_AA);
		putText(mask,to_string(secondMaxLoc),Point(secondMaxLoc-30,rectTop+rectHeight/2), FONT_HERSHEY_SIMPLEX, 0.5,(255,255,255),1,LINE_AA);

		cout << "First max found for top part " << i << ": " << firstMaxVal << " in loc: " << firstMaxLoc << endl;
	    cout << "Second max found for top part " << i << ": " << secondMaxVal << " in loc: " << secondMaxLoc << endl;
		
		int medLoc = firstMaxLoc + (secondMaxLoc - firstMaxLoc)/2;
		cout << "Medium location for top part " << i << ": " << medLoc << " to be compared to " << image.cols/2 << endl;
	}
	addWeighted(image, 1, mask, 0.5, 0, image);

	// Dump the resulting image on disk
    imwrite("output100.jpg", image);
	*/

    return 0;
}

void perspective(Mat & input, Mat & output)
{
	// Il faut 4 points en entrée non alignés et 4 points en sortie pour appliquer une perspective
	Point2f inputQuad[4]; 
	Point2f outputQuad[4];

	// cela permet de générer une matrice de trasnformation
    Mat lambda( 2, 4, CV_32FC1 );

	// Definir les points origine et destination
    inputQuad[0] = Point2f( 0.44*(input.cols-1), 0.63*(input.rows-1) );
    inputQuad[1] = Point2f( 0.57*(input.cols-1), 0.63*(input.rows-1) );
    inputQuad[2] = Point2f( 0.1*(input.cols-1), 0.95*(input.rows-1) );
    inputQuad[3] = Point2f( 1*(input.cols-1), 0.95*(input.rows-1) );
    outputQuad[0] = Point2f( 0, 0 );
    outputQuad[1] = Point2f( input.cols-1, 0 );
    outputQuad[2] = Point2f( 0, input.rows-1 );
    outputQuad[3] = Point2f( input.cols-1, input.rows-1 );

	// Get the Perspective Transform Matrix i.e. lambda 
    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    // Apply the Perspective Transform just found to the src image
    warpPerspective(input, output, lambda, output.size());
}

void histogram(Mat & image, Rect rect, int & firstMaxLoc, int & secondMaxLoc, int & firstMaxVal, int & secondMaxVal)
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


