#pragma once
#ifndef fvkCamera_h__
#define fvkCamera_h__

/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2017/02/09   12:03AM
filename: 	fvkCamera.h
file base:	fvkCamera
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Thread save camera class using OpenCV.
Capturing is done on one thread and captured frame processing on the other.
Both thread are synchronized with semaphore methodology.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkCameraThread.h"
#include "fvkVideoWriter.h"
#include <thread>

namespace R3D
{

class fvkCameraImageProcessing;
class fvkCameraProcessingThread;

class FVK_EXPORT fvkCameraAbstract
{
public:
	virtual void processFrame(cv::Mat& _frame) = 0;
};

class FVK_EXPORT fvkCamera : public fvkCameraAbstract
{

public:
	// Description:
	// Default constructor to create a camera object.
	// _device is the id of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// _resolution is the desired camera frame width and height.
	// resolution Size(-1, -1) will do the auto-selection for the frame's width and height.
	fvkCamera(int _device_id = 0, cv::Size _resolution = cv::Size(-1, -1));
	// Description:
	// Default constructor to create a camera object.
	// _video_file is the location of the video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// _resolution is the desired camera frame width and height.
	// resolution Size(-1, -1) will do the auto-selection for the frame's width and height.
	fvkCamera(const std::string& _video_file, cv::Size _resolution = cv::Size(-1, -1), int _api = cv::VideoCaptureAPIs::CAP_ANY);
	// Description:
	// Constructor to create a camera object by specifying the camera and processing threads.
	// This constructor is created for the derived classes of both fvkCameraThread and fvkCameraProcessingThread.
	fvkCamera(fvkCameraThread* _ct, fvkCameraProcessingThread* _pt);

	// Description:
	// Default destructor that will call the disconnect() function.
	virtual ~fvkCamera();

	// Description:
	// Function that connects the camera device or 
	// if the video file is specified, then it opens the video file.
	// It returns true on success.
	bool connect();
	// Description:
	// Function that returns true if the camera device is connected, otherwise,
	// it always returns false.
	bool isConnected();
	// Description:
	// Function that starts the camera or if the video file is specified,
	// then play the video file.
	// It returns true on success.
	bool start();
	// Description:
	// Function to disconnect the camera device or if the video file is specified,
	// then close the video file.
	// It terminates the camera as well as the processing threads.
	// It returns true on success.
	bool disconnect();

	// Description:
	// Function to pause the camera capturing thread.
	void pause(bool _b);
	// Description:
	// Function that returns true if the camera capturing thread is at pause status.
	bool pause();
	// Description:
	// Set true if you want to restart or repeat the video when it finishes. (only for videos)
	// Default in true.
	void repeat(bool _b);
	// It returns true if the repeat flag for the video is ON. (only for videos)
	// Default in true.
	bool repeat();

	// Description:
	// Function to get the current grabbed frame.
	cv::Mat getFrame() const;

	// Description:
	// Function that returns the average frames per second of the processing thread.
	int getAvgFps() const;
	// Description:
	// Function that returns the total number of processed/passed frames in the processing.
	int getNFrames() const;

	// Description:
	// Function to set the emit function to display the captured frames.
	// The display GUI function should be capable of handling multi-threading updatings.
	void setFrameViewerSlot(const std::function<void(const cv::Mat&)>& _f);
	// Description:
	// Function to set the emit function to get the average frames per second of this thread
	// as well as the total number of frames that has been processed/passed.
	// The input GUI function should be capable of handling multi-threading updatings.
	void setFrameStatisticsSlot(const std::function<void(const fvkAverageFpsStats&)>& _f);
	
	// Description:
	// Function to get a pointer to video recorder.
	fvkVideoWriter* getRecorder() const;

	// Description:
	// Function to get a pointer to image processing.
	fvkCameraImageProcessing* imageProcessing() const;

	// Description:
	// Function that saves the current image frame
	// to the specified output location setFrameOutputLocation("D:\\frame.jpg").
	// Note: The folder should have writable permission, such as Pictures/Videos/Documents folder.
	// Example:
	// p->setFrameOutputLocation("D:\\frame.jpg");
	// p->saveFrameOnClick();
	void saveFrameOnClick();
	// Description:
	// Function to set the output location for the frame to be saved.
	// The path folder should have writable permission, such as Pictures/Videos/Documents folder.
	// Example:
	// p->setFrameOutputLocation("D:\\frame.jpg");
	void setFrameOutputLocation(const std::string& _filename);
	// Description:
	// Function to get the output location for the frame to be saved.
	std::string getFrameOutputLocation() const;

	// Description:
	// Function to set the video file path like "D:\\face\\video.avi". (only for videos)
	// If the video file is specified before the calling connect() function, then the specified video
	// will be played, rather than camera device.
	void setVideoFile(const std::string& _filename);
	// Description:
	// Function to get the video file path. (only for videos)
	std::string getVideoFile() const;

	// Description:
	// Function to set the camera device index.
	// _id is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	void setDeviceIndex(int _index);
	// Description:
	// Function to get the camera device index.
	int getDeviceIndex() const;

	// Description:
	// Function to set the frame delay which makes specified delay in the camera thread.
	// Default delay is 33 milliseconds which is for the camera having 3DFPS capturing speed.
	void setFrameDelay(int _delay_msec);
	// Description:
	// Function to get the frame delay.
	// Default delay for video files is computed by (1000.0 / getFps()). (only for videos)
	int getFrameDelay() const;

	// Description:
	// Function to enable the perfect synchronization between the processing thread and the camera thread.
	// If it's true, this thread will remain be blocked until the processing thread notify this thread.
	void setSyncEnabled(bool _b);
	// Description:
	// Function that returns true if the perfect synchronization is enabled.
	bool isSyncEnabled();

	/************************************************************************/
	/* Camera properties                                                    */
	/************************************************************************/
	// Description:
	// Call this function in order to open the camera driver config dialog.
	void openDriverConfigDialog();
	// Description:
	// Function to set the width of the captured frame.
	// Width should be specified before calling the open() function.
	void setWidth(int _w);
	// Description:
	// Function to get the width of the captured frame.
	int getWidth() const;
	// Description:
	// Function to set the height of the captured frame.
	// Height should be specified before calling the open() function.
	void setHeight(int _h);
	// Description:
	// Function to get the height of the captured frame.
	int getHeight() const;
	// Description:
	// Function to set the camera resolution (columns and rows).
	// Resolution should be specified before calling the open() function.
	void setResolution(cv::Size _res);
	// Description:
	// Function to get the current camera resolution.
	cv::Size getResolution() const;
	// Description:
	// Current position of the video file in milliseconds or video capture time-stamp.
	void setMsec(double _v);
	double getMsec() const;
	// Description:
	// 0-based index of the frame to be decoded/captured next.
	void setPosFrames(double _v);
	double getPosFrames() const;
	// Description:
	// Relative position of the video file: 0 - start of the film, 1 - end of the film.
	void setAviRatio(double _v);
	double getAviRatio() const;
	// Description:
	// desired camera frame rate.
	void setFps(double _v);
	double getFps() const;;
	// Description:
	// Function to set a 4-character code of codec.
	void setFourCC(double _v);
	// Description:
	// Function that returns the 4-character code that identify a video codec.
	std::string getFourCC() const;
	// Description:
	// Number of frames in the video file.
	void setFrameCount(double _v);
	double getFrameCount() const;
	// Description:
	// Format of the Mat objects returned by retrieve().
	void setFormat(double _v);
	double getFormat() const;
	// Description:
	// Backend-specific value indicating the current capture mode.
	void setMode(double _v);
	double getMode() const;

	// Description:
	// Brightness of the image (only for cameras)..
	void setBrightness(double val);
	double getBrightness() const;
	// Description:
	// Contrast of the image (only for cameras).
	void setContrast(double val);
	double getContrast() const;
	// Description:
	// Saturation of the image (only for cameras).
	void setSaturation(double val);
	double getSaturation() const;
	// Description:
	//  Hue of the image (only for cameras).
	void setHue(double val);
	double getHue() const;
	// Description:
	// Gain of the image (only for cameras).
	void setGain(double val);
	double getGain() const;
	// Description:
	// Exposure (only for cameras).
	void setExposure(double val);
	double getExposure() const;
	// Description:
	// Temperature.
	void setTemperature(double val);
	double getTemperature() const;
	// Description:
	// Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
	void setRectification(double val);
	double getRectification() const;
	// Description:
	// Gamma.
	void setGamma(double val);
	double getGamma() const;
	// Description:
	// Set the value for camera sharpness. Specify zero in order to disable sharpness.
	void setSharpness(double val);
	double getSharpness() const;
	// Description:
	// Set 1 for auto-exposure and 0 for disabling it.
	void setAutoExposure(double val);
	double getAutoExposure() const;
	// Description:
	// Set 0 for disabling auto-white balance
	void setWhiteBalanceBlueU(double val);
	double getWhiteBalanceBlueU() const;
	// Description:
	// Set 0 for disabling auto-white balance
	void setWhiteBalanceRedV(double val);
	double getWhiteBalanceRedV() const;
	// Description:
	// Boolean flags indicating whether images should be converted to RGB.
	void setConvertToRGB(double val);
	double getConvertToRGB() const;

	// Description:
	// Set zoom.
	void setZoom(double val);
	double getZoom() const;
	// Description:
	// Set focus.
	void setFocus(double val);
	double getFocus() const;
	// Description:
	// Set pan.
	void setPan(double val);
	double getPan() const;
	// Description:
	// Set tilt.
	void setTilt(double val);
	double getTilt() const;
	// Description:
	// Set roll.
	void setRoll(double val);
	double getRoll() const;
	// Description:
	// The ISO speed of the camera (note: only supported by DC1394 v 2.x backend currently)
	void setIsoSpeed(double val);
	double getIsoSpeed() const;
	// Description:
	// Set back light.
	void setBackLight(double val);
	double getBackLight() const;
	// Description:
	// Set trigger.
	void setTrigger(double val);
	double getTrigger() const;
	// Description:
	// Set trigger delay.
	void setTriggerDelay(double val);
	double getTriggerDelay() const;

protected:
	// Description:
	// Only virtual function that is expected to be overridden in the derived class
	// to process the captured frame.
	virtual void processFrame(cv::Mat& _frame) override;

	fvkCameraThread *p_ct;
	fvkCameraProcessingThread *p_pt;
	std::thread* p_stdct;
	std::thread* p_stdpt;
};

}

#endif // fvkCamera_h__