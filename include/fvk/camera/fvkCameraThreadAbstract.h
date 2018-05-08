#pragma once
#ifndef fvkCameraThreadAbstract_h__
#define fvkCameraThreadAbstract_h__

/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2017/02/09   11:18PM
filename: 	fvkCameraThreadAbstract.h
file base:	fvkCameraThreadAbstract
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	OpenCV camera class interface.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkExport.h"

#include "opencv2/opencv.hpp"

namespace R3D
{

class FVK_EXPORT fvkCameraThreadAbstract
{

public:
	// Description:
	// Default constructor to initialize the data members.
	fvkCameraThreadAbstract();
	// Description:
	// Default destructor that does nothing in this abstract class.
	virtual ~fvkCameraThreadAbstract();

	// Description:
	// Function to connect the camera device.
	//  _device_index is the index of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// It returns true on success.
	virtual auto open(const int _device_index) -> bool = 0;
	// Description:
	// Function to connect the camera device.
	// It returns true on success.
	virtual auto open() -> bool;
	// Description:
	// It returns true if the camera device is successfully connected.
	virtual auto isOpened() const -> bool = 0;
	// Description:
	// It returns true if the camera device is successfully disconnected.
	virtual auto close() -> bool = 0;

	// Description:
	//  Virtual function to be overridden to get the current grabbed frame.
	virtual auto getFrame() const -> cv::Mat = 0;

	// Description:
	// Function to set the camera device index.
	//  _device is the id of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	void setDeviceIndex(const int _index) { m_device_index = _index; }
	// Description:
	// Function to get the camera device index.
	auto getDeviceIndex() const { return m_device_index; }

	// Description:
	// Function to set the width of the captured frame.
	void setFrameWidth(const int _w) { m_frame_size.width = _w; }
	// Description:
	// Function to get the width of the captured frame.
	auto getFrameWidth() const { return m_frame_size.width; };
	// Description:
	// Function to set the height of the captured frame.
	void setFrameHeight(const int _h) { m_frame_size.height = _h; }
	// Description:
	// Function to get the height of the captured frame.
	auto getFrameHeight() const { return m_frame_size.height; };
	// Description:
	// Function to set the camera resolution (columns and rows).
	// Resolution should be specified before calling the open() function.
	void setFrameSize(const cv::Size& _size) { m_frame_size = _size; }
	// Description:
	// Function to get the current camera resolution.
	auto getFrameSize() const { return m_frame_size; };

protected:
	// Description:
	// Pure virtual function to be overridden to grab/capture the frame. 
	virtual auto grab(cv::Mat& _frame) -> bool = 0;

	int m_device_index;
	cv::Size m_frame_size;
};

}

#endif // fvkCameraThreadAbstract_h__
