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

purpose:	Class to create a thread for capturing frames from OpenCV camera.

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
#include "fvkAverageFps.h"
#include <functional>

namespace R3D
{

class FVK_EXPORT fvkCameraThread : public fvkCameraThreadAbstract
{
public:
	// Description:
	// Default constructor for start the camera device.
	//  _device_id is the index number of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// If _width and _height is specified, then this will become the camera frame resolution.
	// _buffer is the semaphore to synchronizer the processing thread with this thread.
	// cv::Size(-1, -1) will do the auto-selection of the resolution, normally it enables the 640x480 resolution.
	fvkCameraThread(fvkSemaphoreBuffer<cv::Mat>* _buffer, int _device_id, cv::Size _resolution = cv::Size(-1, -1));
	// Description:
	// Default constructor to start the given video file.
	// _buffer is the semaphore to synchronizer the processing thread with this thread.
	// _video_file is the absolute path of the video file.
	// If _width and _height is specified, then this will become the video frame resolution.
	// cv::Size(-1, -1) will do the auto-selection of the resolution, normally it enable the 640x480 resolution.
	// _api is the preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	fvkCameraThread(fvkSemaphoreBuffer<cv::Mat>* _buffer, const std::string& _video_file, cv::Size _resolution = cv::Size(-1, -1), int _api = cv::VideoCaptureAPIs::CAP_ANY);
	// Description:
	// Default destructor to stop the thread and releases the camera device or video file.
	virtual ~fvkCameraThread();

	// Description:
	// Function that starts the camera loop.
	virtual void run() override;

	// Description:
	// Overridden function to pause the camera capturing thread.
	virtual void pause(bool _b) override;
	// Description:
	// Overridden function that returns true if the camera capturing thread is on pause state.
	virtual bool pause() override;
	// Description:
	// Function to stop the camera capturing thread.
	virtual void stop() override;
	// Description:
	// Set true if you want to restart or repeat the video when it finishes. (only for videos)
	// Default in true.
	virtual void repeat(bool _b) override;
	// It returns true if the repeat flag for the video is on. (only for videos)
	// Default in true.
	virtual bool repeat() override;
	// Description:
	// Function to get the current grabbed frame.
	virtual cv::Mat getFrame() override;

	// Description:
	// Function that returns the average frames per second of this thread.
	int getAvgFps();
	// Description:
	// Function that returns the total number of process/passed frames.
	int getNFrames();
	// Description:
	// Set the emit function to get the average frames per second of this thread
	// as well as the total number of frames that has been processed/passed.
	// The input function should be capable of handling multi-threading updating.
	void setFrameStatisticsSlot(std::function<void(const fvkAverageFpsStats&)> _f) { emit_stats = _f; }

	// Description:
	// Function to set a pointer to semaphore buffer which does synchronization between capturing and processing threads.
	void setSemaphoreBuffer(fvkSemaphoreBuffer<cv::Mat>* _p) { p_buffer = _p; }
	// Description:
	// Function to get a pointer to semaphore buffer which does synchronization between capturing and processing threads.
	fvkSemaphoreBuffer<cv::Mat>* getSemaphoreBuffer() const { return p_buffer; }

	// Description:
	// Function to enable the perfect synchronization between the processing thread and the camera thread.
	// If it's true, this thread will remain be blocked until the processing thread notify this thread.
	// Default value is false. So, in that case, camera thread will keep running/grabbing and just wait to get notify from
	// the processing thread to add the grabbed frame in thq queue.
	void setSyncEnabled(bool _b);
	// Description:
	// Function that returns true if the perfect synchronization is enabled.
	bool isSyncEnabled();

protected:
	// Description:
	// Overridden function to grab frames from the camera device or the video file.
	// If the video file path is specified, then this function will try to grab frames from the video file,
	// otherwise capturing from camera device will be ON. 
	// In order to grab from the camera device, the video file path
	// should be empty, like setVideoFile("");
	virtual bool grab(cv::Mat& _m_frame) override;

	std::mutex m_stopmutex;
	std::mutex m_pausemutex;
	std::mutex m_repeatmutex;
	std::condition_variable m_pausecond;
	fvkSemaphoreBuffer<cv::Mat> *p_buffer;

	std::function<void(const fvkAverageFpsStats&)> emit_stats;
	fvkAverageFps m_avgfps;
	std::mutex m_statsmutex;
	std::mutex m_syncmutex;
	bool m_sync_proc_thread;
};

}

#endif // fvkCameraThread_h__