#pragma once
#ifndef fvkCamera_h__
#define fvkCamera_h__

/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2018/05/07   12:03AM
filename: 	fvkCamera.h
file base:	fvkCamera
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Thread-safe multi-threaded camera class for any type of camera device.
Capturing is done on one thread and captured frame processing on the other.
Both threads are synchronized with semaphore methodology.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkCameraThreadOpenCV.h"
#include "fvkProcessingThread.h"
#include <thread>

namespace R3D
{

class FVK_CAMERA_EXPORT fvkCameraAbstract
{
public:
	// Description:
	// Default virtual destructor.
	virtual ~fvkCameraAbstract() = default;

	// Description:
	// Virtual function that is expected to be overridden in the derived class in order
	// to process the captured frame.
	virtual void present(cv::Mat& _frame) = 0;
};

class FVK_CAMERA_EXPORT fvkCamera : public fvkCameraAbstract
{

public:
	// Description:
	// Default constructor that creates a camera object for continuous streaming.
	// _device_index is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// _frame_size is the desired width and height of camera frame.
	// Specifying Size(-1, -1) will do the auto-selection for the captured frame size,
	// normally it enables the 640x480 resolution on most of web cams.
	// _api is the video capture API to enable various types of devices.
	explicit fvkCamera(const int _device_index = 0, const cv::Size& _frame_size = cv::Size(-1, -1), const int _api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY));
	// Description:
	// Default constructor that creates a camera object for continuous streaming.
	// _video_file is the location of the video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// _frame_size is the desired camera frame width and height.
	// Specifying Size(-1, -1) will do the auto-selection for the frame's width and height.
	// _api is the video capture API to enable various types of devices.
	explicit fvkCamera(const std::string& _video_file, const cv::Size& _frame_size = cv::Size(-1, -1), const int _api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY));
	// Description:
	// Constructor that creates a camera object by specifying the camera thread.
	// This constructor is created for the derived classes of fvkCameraThread.
	explicit fvkCamera(fvkCameraThread* _ct);
	// Description:
	// Constructor to create a camera object by specifying the camera and processing threads.
	// This constructor is created for the derived classes of both fvkCameraThread and fvkCameraProcessingThread.
	fvkCamera(fvkCameraThread* _ct, fvkProcessingThread* _pt);
	// Description:
	// Default destructor that stops the threads and closes the camera device.
	virtual ~fvkCamera();

	// Description:
	// Function that connects the camera device or 
	// if the video file is specified, then it opens the video file.
	// It returns true on success.
	auto connect() -> bool;
	// Description:
	// Function that returns true if the camera device is connected, otherwise,
	// it always returns false.
	auto isConnected() const -> bool;
	// Description:
	// Function that starts the camera or if the video file is specified,
	// then play the video file.
	// It returns true on success.
	auto start() -> bool;
	// Description:
	// Function to disconnect the camera device or if the video file is specified,
	// then close the video file.
	// It terminates the camera as well as the processing threads.
	// It returns true on success.
	auto disconnect() -> bool;

	// Description:
	// Function to set the camera device index.
	// _id is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	void setDeviceIndex(const int _index) const;
	// Description:
	// Function to get the camera device index.
	auto getDeviceIndex() const -> int;
	// Description:
	// Function to set the captured frame size (columns/width and rows/height).
	// Frame size must be specified before calling the open() function, otherwise
	// auto frame size will be selected by the camera device.
	void setFrameSize(const cv::Size& _size) const;
	// Description:
	// Function to get the captured frame size.
	auto getFrameSize() const -> cv::Size;

	// Description:
	// Function to pause the camera capturing thread.
	void pause(const bool _b) const;
	// Description:
	// Function that returns true if the camera capturing thread is at pause status.
	auto pause() const -> bool;

	// Description:
	// Function to set the frame delay which makes specified delay between frames in the camera thread.
	// Default delay is 33 milliseconds which is for the camera having 30 FPS capturing speed.
	void setDelay(const int _delay_msec) const;
	// Description:
	// Function to get the frame delay.
	// Default delay for video files is computed by (1000.0 / getFps()). (only for videos)
	auto getDelay() const -> int;

	// Description:
	// Function to enable the perfect synchronization between the processing thread and the camera thread.
	// If it's true, this thread will remain be blocked until the processing thread notify this thread.
	void setSyncEnabled(const bool _b) const;
	// Description:
	// Function that returns true if the perfect synchronization is enabled.
	auto isSyncEnabled() const -> bool;

	// Description:
	// Function to get the current grabbed frame.
	auto getFrame() const -> cv::Mat;

	// Description:
	// Function that returns the average frames per second of the processing thread.
	auto getAvgFps() const -> int;
	// Description:
	// Function that returns the total number of processed/passed frames in the processing.
	auto getFrameNumber() const -> int;

	// Description:
	// Set a GUI function to display the grabbed frame.
	// The display function should be capable of handling multi-threading updating.
	// The second argument which is fvkThreadStats will give you statistics of the Processing thread,
	// such as Average frames per second (FPS) and number of processed frames.
	void setVideoOutput(const std::function<void(cv::Mat&, const fvkThreadStats&)> _f) const;

	// Description:
	// Function that saves the current image frame
	// to the specified output location setFrameOutputLocation("D:\\frame.jpg").
	// Note: The folder should have writable permission, such as Pictures/Videos/Documents folder.
	// Example:
	// p->setFrameOutputLocation("D:\\frame.jpg");
	// p->saveFrameOnClick();
	void saveFrameOnClick() const;
	// Description:
	// Function to set the output location for the frame to be saved.
	// The path folder should have writable permission, such as Pictures/Videos/Documents folder.
	// Example:
	// p->setFrameOutputLocation("D:\\frame.jpg");
	void setFrameOutputLocation(const std::string& _filename) const;
	// Description:
	// Function to get the output location for the frame to be saved.
	auto getFrameOutputLocation() const -> std::string;

	// Description:
	// Function to set the video file path like "D:\\face\\video.avi".
	// If the video file is specified before the calling connect() function, then the specified video
	// will be played, rather than camera device.
	// This function is only for videos.
	void setVideoFileLocation(const std::string& _filename) const;
	// Description:
	// Function to get the video file path.
	// This function is only for videos.
	auto getVideoFileLocation() const -> std::string;
	// Description:
	// Set true if you want to restart or repeat the video when it finishes.
	// Default in true.
	// This function is only for videos.
	void repeat(const bool _b) const;
	// It returns true if the repeat flag for the video is on.
	// This function is only for videos.
	// Default in true.
	auto repeat() const -> bool;

	// Description:
	// Function to set the Video Capture preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	// For example:
	// To use DirectShow, specify cv::CAP_DSHOW.
	// To use CAP_FFMPEG, specify cv::CAP_FFMPEG.
	// To use Microsoft Media Foundation, specify cv::CAP_MSMF. On Windows, this is the default API.
	void setAPI(const int _api) const;
	// Description:
	// Function to get the Video Capture preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	auto getAPI() const -> int;

	// Description:
	// Calling this function opens a camera configuration dialog, if supported by the camera driver.
	virtual void openConfigurationDialog();

	// Description:
	// Function to get a reference to video writer.
	// It is a helper shortcut function to "cam->getCameraProcessingThread()->writer()".
	auto& writer() { return p_pt->writer(); }
	// Description:
	// Function to get a reference to image processing.
	// It is a helper shortcut function to "cam->getCameraProcessingThread()->imageProcessing()".
	auto& imageProcessing() { return p_pt->imageProcessing(); }

	// Description:
	// Function to get a pointer to camera/capturing thread.
	auto getCamThread() const { return p_ct; }
	// Description:
	// Function to get a pointer to camera/frame processing thread.
	auto getProcThread() const { return p_pt; }

	// Description:
	// Function to get a pointer to camera/capturing thread handle.
	auto getCamThreadHandle() const { return m_ct_handle; }
	// Description:
	// Function to get a pointer to camera/frame processing thread handle.
	auto getProcThreadHandle() const { return m_pt_handle; }

protected:
	// Description:
	// Virtual function that is expected to be overridden in the derived class in order
	// to process the captured frame.
	void present(cv::Mat& _frame) override;

	fvkCameraThread* p_ct;			// camera runs on capturing thread.
	fvkProcessingThread* p_pt;		// captured frame processing runs on processing thread.

	std::thread* p_stdct;			// thread for capturing device.
	std::thread* p_stdpt;			// thread for captured frame processing.
	void* m_ct_handle;				// native handle for capturing thread.
	void* m_pt_handle;				// native handle for processing thread.
};

}

#endif // fvkCamera_h__