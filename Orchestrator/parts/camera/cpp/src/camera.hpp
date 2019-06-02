#ifndef SOATCAMERAPART_H
#define SOATCAMERAPART_H

#include <raspicam/raspicam.h>
#include "SoatcarState.hpp"
#include "config.hpp"


class CameraPart
{
	public:
		CameraPart();
		void run();
		
	private:
		SoatcarState state{"/var/tmp/soatcarmmf.tmp"};
		CameraConfig config{"camera.conf"};
		unsigned long long nbInput;
		raspicam::RaspiCam camera;
		DataBuf image;
		
		void openCameraDevice();
		void grabImage();
		void closeCameraDevice();
};

#endif