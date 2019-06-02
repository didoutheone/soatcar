#include <iostream>			// for cout, cerr
#include <raspicam/raspicam.h>
#include <thread>       	// for this_thread

#include "camera.hpp"
#include "SoatcarState.hpp"
#include "Utils.hpp"

using namespace std;

CameraPart::CameraPart()
{
	nbInput = 0;
	
	cout << "Initializing Camera..." << endl;

	openCameraDevice();
	
    cout << "Camera N°" << camera.getId() << " initialized successfully" << endl;
	
	state.setCameraReady(true);
}

void CameraPart::run()
{
	while(!state.getStopFlag())
	{
		clock_type::time_point begin = Utils::getBeginChrono();
		
		grabImage();
		
		// Write image in state
		state.setRawImage(image);
		state.setRawImageNo(++nbInput);
		
		this_thread::sleep_for(chrono::milliseconds(5));
		int freq = Utils::getFrequency(begin);
		state.setCameraLoopRate(freq);
	}
	
	closeCameraDevice();
	state.setCameraReady(false);
}

void CameraPart::openCameraDevice()
{
	// Set camera params
	camera.setWidth(config.Width);
	camera.setHeight(config.Height);
	camera.setFrameRate(config.FPS);
	camera.setFormat(raspicam::RASPICAM_FORMAT_RGB);
	
	if (!camera.open())
	{
		cerr << "Error opening the camera" << endl;
		exit(-1);
	}
	
	// init databuf
	image.length = camera.getImageBufferSize();
	image.buffer = new u8[image.length];
	
	// Wait 3s for camera stabilization
	this_thread::sleep_for(chrono::milliseconds(3000));
}

void CameraPart::grabImage()
{
	// Grab next frame
	camera.grab();
	
	// Extract the image
	camera.retrieve(image.buffer);
}

void CameraPart::closeCameraDevice()
{
	camera.release();
	delete image.buffer;
}
