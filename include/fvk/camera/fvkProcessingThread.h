#pragma once
#ifndef fvkProcessingThread_h__
#define fvkProcessingThread_h__

/*********************************************************************************
created:	2016/01/10   01:37AM
modified:	2018/05/16   11:48PM
filename: 	fvkProcessingThread.h
file base:	fvkProcessingThread
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class that create a parallel thread for the processing of the grabbed
camera frame. This thread is synchronized with the camera thread using a semaphore.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkImageProcessing.h"
#include "fvkSemaphoreBuffer.h"
#include "fvkVideoWriter.h"
#include "fvkThread.h"

namespace R3D
{

class fvkCameraAbstract;

class FVK_CAMERA_EXPORT fvkProcessingThread : public fvkThread
{
public:
	// Description:
	// Default constructor to create a synchronized processing thread.
	// _device_index is the id of the opened video capturing device (i.e. a camera index).
	// _frameobserver is the parent class of Camera that will override the present function.
	// _buffer is the semaphore to synchronize the camera thread with this thread.
	fvkProcessingThread(const int device_index, fvkCameraAbstract* frameobserver, fvkSemaphoreBuffer<cv::Mat>* buffer = nullptr);
	// Description:
	// Default constructor to create a synchronized processing thread.
	// _device_index is the id of the opened video capturing device (i.e. a camera index).
	// _buffer is the semaphore to synchronize the camera thread with this thread.
	explicit fvkProcessingThread(const int device_index, fvkSemaphoreBuffer<cv::Mat>* buffer = nullptr);
	// Description:
	// Default destructor to stop the thread as well as recorder, and delete the data.
	virtual ~fvkProcessingThread();

	// Description:
	// Overridden function to get the current grabbed frame.
	auto getFrame() -> cv::Mat;

	// Description:
	// Function to set the camera device id.
	//  _device is the id of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	void setDeviceIndex(const int index) { m_device_index = index; }
	// Description:
	// Function to get the camera device id.
	auto getDeviceIndex() const { return m_device_index; }

	// Description:
	// Function to set the saved frame file path with file name and extension.
	// The path folder should have writable permission, such as Pictures/Videos/Documents folder.
	// Example:
	// p->setFrameOutputLocation("D:\\frame.jpg");
	void setFrameOutputLocation(const std::string& path) { m_filepath = path; }
	// Description:
	// Function to get the saved frame file path.
	auto getFrameOutputLocation() const { return m_filepath; }
	// Description:
	// Function that trigger the saveFrameToDisk function to save the current image frame
	// at the specified output location setFrameOutputLocation("D:\\frame.jpg").
	// Note: The folder should have writable permission, such as Pictures/Videos/Documents folder.
	// Example:
	// p->setFrameOutputLocation("D:\\frame.jpg");
	// p->saveFrameOnClick();
	void saveFrameOnClick();

	// Description:
	// Set a GUI function to display the grabbed frame.
	// The display function should be capable of handling multi-threading updating.
	// The second argument which is fvkThreadStats will give you statistics of the thread,
	// such as Average frames per second (FPS) and number of processed frames.
	void setVideoOutput(const std::function<void(cv::Mat&, const fvkThreadStats&)> f);

	// Description:
	// Function to set a pointer to semaphore buffer which does synchronization between capturing and processing threads.
	void setSemaphoreBuffer(fvkSemaphoreBuffer<cv::Mat>* p) { p_buffer = p; }
	// Description:
	// Function to get a pointer to semaphore buffer which does synchronization between capturing and processing threads.
	auto getSemaphoreBuffer() const { return p_buffer; }

	// Description:
	// Function to get a reference to video writer.
	auto& writer() { return m_vr; }

	// Description:
	// Function to get a reference to image processing.
	auto& imageProcessing() { return m_ip; }

protected:
	// Description:
	// Overridden function to process the camera frame.
	void run() override;

	// Description:
	// Virtual function that is expected to be overridden in the derived class in order
	// to process the captured frame.
	virtual void present(cv::Mat& frame);

	// Description:
	// Function that saves the current frame to disk (file path must be specified by setSavedFile("")).
	auto saveFrameToDisk(const cv::Mat& frame) -> bool;

	// Description:
	// protected member variables.
	fvkCameraAbstract *p_frameobserver;
	std::mutex m_processing_mutex;
	fvkSemaphoreBuffer<cv::Mat> *p_buffer;
	std::function<void(cv::Mat&, const fvkThreadStats&)> m_video_output_func;

	fvkImageProcessing m_ip;
	fvkVideoWriter m_vr;

	int m_device_index;
	std::string m_filepath;
	std::atomic<bool> m_save;
};

}

#endif // fvkProcessingThread_h__