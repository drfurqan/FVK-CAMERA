/*********************************************************************************
created:	2017/07/28   02:47AM
filename: 	camera_program_2.cpp
file base:	camera_program_2
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Simplest program to run a camera device. It uses OpenCV window to show
the grabbed image frames.
Second method to set a function to show the grabbed frames.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkCamera.h>
#include <fvk/camera/fvkCameraImageProcessing.h>

using namespace R3D;

// creating a function that will be calling in the continuous running camera thread.
// _frame is the grabbed frame.
static void show(const cv::Mat& _frame)
{
	cv::Mat m = _frame.clone();			// always create a clone to process the frame.
	cv::cvtColor(m, m, CV_BGR2GRAY);	// just doing the simple image processing that converts to gray-scale image.
	cv::imshow("FVK Camera", m);
}

int main()
{
	// create an object for the camera device
	// first argument is the id of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// second argument is the desired camera frame width and height.
	// resolution Size(-1, -1) will do the auto-selection for the frame's width and height.
	fvkCamera cam(0, cv::Size(640, 480));

	// try to connect the camera device and initialize it.
	if (cam.connect())
	{
		// start the camera threads (frame grabbing + processing thread).
		if (cam.start())
		{
			std::cout << "camera has started!" << std::endl;
		}
	}

	// create a window using OpenCV.
	cv::namedWindow("FVK Camera");

	// using std::bing to bind the given show() function with the camera thread.
	cam.setFrameViewerSlot(std::bind(&show, std::placeholders::_1));

	// OpenCV event loop that will prevent to exit the main loop.
	cv::waitKey(0);

	return 1;
}