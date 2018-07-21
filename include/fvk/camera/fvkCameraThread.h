#pragma once
#ifndef fvkCameraThread_h__
#define fvkCameraThread_h__

/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2017/02/09   11:41PM
filename: 	fvkCameraThread.h
file base:	fvkCameraThread
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class to create a camera capturing thread. This class can be further
extended in order to make any camera device compatible to this library.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkCameraThreadAbstract.h"
#include "fvkSemaphoreBuffer.h"
#include "fvkThread.h"

namespace R3D
{

class FVK_CAMERA_EXPORT fvkCameraThread : public fvkThread, public fvkCameraThreadAbstract
{
public:
	// Description:
	// Default constructor that creates a camera thread for continuous streaming.
	// _device_index is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// _frame_size is the desired width and height of camera frame.
	// Specifying Size(-1, -1) will do the auto-selection for the captured frame size,
	// normally it enables the 640x480 resolution on most of web cams.
	fvkCameraThread(const int _device_index, const cv::Size& _frame_size, fvkSemaphoreBuffer<cv::Mat>* _buffer = nullptr);
	// Description:
	// Default destructor that expected to be overridden.
	virtual ~fvkCameraThread() = default;

	// Description:
	// Function to get the current grabbed frame.
	auto getFrame() -> cv::Mat override;

	// Description:
	// Set a GUI function to display the grabbed frame.
	// The display function should be capable of handling multi-threading updating.
	// The second argument which is fvkThreadStats will give you statistics of the thread,
	// such as Average frames per second (FPS) and number of processed frames.
	void setVideoOutput(const std::function<void(cv::Mat&, const fvkThreadStats&)> _f);

	// Description:
	// Function to set a pointer to semaphore buffer which does synchronization between capturing and processing threads.
	void setSemaphoreBuffer(fvkSemaphoreBuffer<cv::Mat>* _p) { p_buffer = _p; }
	// Description:
	// Function to get a pointer to semaphore buffer which does synchronization between capturing and processing threads.
	auto getSemaphoreBuffer() const { return p_buffer; }

	// Description:
	// Function to set the region-of-interest of the grabbed frame.
	void setRoi(const cv::Rect& _roi);
	// Description:
	// Function to get the region-of-interest of the grabbed frame.
	auto getRoi() -> cv::Rect;

	// Description:
	// Function to enable the buffer synchronization between the processing thread and the camera thread.
	// If it's true, this thread will remain be blocked until the processing thread notify this thread.
	// Default value is false. So, in that case, camera thread will keep running/grabbing and just wait to get notify from
	// the processing thread to add the grabbed frame in the queue.
	void setSyncEnabled(const bool _b);
	// Description:
	// Function that returns true if the buffer synchronization is enabled.
	auto isSyncEnabled() const -> bool;

protected:	
	// Description:
	// Overridden function to grab and process the camera frame.
	void run() override final;

	// Description:
	// Pure virtual function to be overridden to grab/capture the frame. 
	auto grab(cv::Mat& _frame) -> bool override = 0;

	// Description:
	// protected member variables.
	fvkSemaphoreBuffer<cv::Mat> *p_buffer;
	std::function<void(cv::Mat&, const fvkThreadStats&)> m_video_output_func;
	std::mutex m_syncmutex;
	std::mutex m_repeatmutex;
	std::atomic<bool> m_sync_proc_thread;
	std::mutex m_rectmutex;
	cv::Rect m_rect;
};

}

#endif // fvkCameraThread_h__