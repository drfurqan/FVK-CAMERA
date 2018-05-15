#pragma once
#ifndef fvkCameraThreadOpenCV_h__
#define fvkCameraThreadOpenCV_h__

/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2018/05/07   11:18PM
filename: 	fvkCameraThreadOpenCV.h
file base:	fvkCameraThreadOpenCV
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class to create handle opencv video camera.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkExport.h"

#include "fvkCameraThread.h"

namespace R3D
{

class FVK_EXPORT fvkCameraThreadOpenCV : public fvkCameraThread
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
	// _buffer is the semaphore buffer to synchronizer the processing thread with this camera thread.
	fvkCameraThreadOpenCV(const int _device_index, const cv::Size& _resolution, fvkSemaphoreBuffer<cv::Mat>* _buffer = nullptr);
	// Description:
	// Default constructor to start the given video file.
	// _buffer is the semaphore to synchronizer the processing thread with this thread.
	// _video_file is the absolute path of the video file.
	// If _width and _height is specified, then this will become the video frame resolution.
	// cv::Size(-1, -1) will do the auto-selection of the resolution, normally it enable the 640x480 resolution.
	// _api = cv::VideoCaptureAPIs::CAP_ANY is the preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	fvkCameraThreadOpenCV(const std::string& _video_file, const cv::Size& _resolution, const int _api, fvkSemaphoreBuffer<cv::Mat>* _buffer = nullptr);
	// Description:
	// Default destructor that stops the threads and closes the camera device.
	virtual ~fvkCameraThreadOpenCV();

	// Description:
	// Function to connect the camera device.
	// _device_index is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// It returns true on success.
	auto open(const int _device_index) -> bool override;
	// Description:
	// Function to connect the video file.
	// _file_name is the name of the opened video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// _api is the preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	// It returns true on success.
	virtual auto open(const std::string& _file_name, const int _api = cv::VideoCaptureAPIs::CAP_ANY) -> bool;
	// Description:
	// Function to connect the camera.
	// If the video file is specified before the calling this function, then this method will try to 
	// open the video file, otherwise it will open the camera device.
	// It returns true on success.
	auto open() -> bool override;;
	// Description:
	// Function that returns true if the camera or video file is successfully connected.
	auto isOpened() const -> bool override;;
	// Description:
	// Function that returns true if the camera or video file is successfully disconnected.
	auto close() -> bool override;;

	// Description:
	// Set true if you want to restart or repeat the video when it finishes. (only for videos)
	// Default in true.
	void repeat(const bool _b);
	// It returns true if the repeat flag for the video is on. (only for videos)
	// Default in true.
	auto repeat() const -> bool;

	// Description:
	// Function to set the Video Capture preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	void setAPI(const int _api) { m_videocapture_api = _api; }
	// Description:
	// Function to get the Video Capture preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	auto getAPI() const { return m_videocapture_api; }

	// Description:
	// Function to set the video file path like "D:\\face\\video.avi". (only for videos)
	// If the video file is specified before the calling connect() function, then the specified video
	// will be played, rather than camera device.
	void setVideoFile(const std::string& _filename) { m_filepath = _filename; }
	// Description:
	// Function to get the video file path. (only for videos)
	auto getVideoFile() const { return m_filepath; }

	/************************************************************************/
	/* Camera properties                                                    */
	/************************************************************************/
	// Description:
	// Call this function in order to open the camera driver config dialog.
	void openDriverConfigDialog();

	// Description:
	// Function to get the current camera resolution.
	auto getActiveFrameSize() const -> cv::Size;

	// Description:
	// Current position of the video file in milliseconds or video capture timestamp.
	void setMsec(double _v);
	auto getMsec() const -> double;
	// Description:
	// 0-based index of the frame to be decoded/captured next.
	void setPosFrames(double _v);
	auto getPosFrames() const -> double;
	// Description:
	// Relative position of the video file: 0 - start of the film, 1 - end of the film.
	void setAviRatio(double _v);
	auto getAviRatio() const -> double;
	// Description:
	// desired camera frame rate.
	void setFps(double _v);
	auto getFps() const -> double;
	// Description:
	// Function to set a 4-character code of codec.
	void setFourCC(double _v);
	// Description:
	// Function that returns the 4-character code that identify a video codec.
	auto getFourCC() const -> std::string;
	// Description:
	// Number of frames in the video file.
	void setFrameCount(double _v);
	auto getFrameCount() const -> double;
	// Description:
	// Format of the Mat objects returned by retrieve().
	void setFormat(double _v);
	auto getFormat() const -> double;
	// Description:
	// Backend-specific value indicating the current capture mode.
	void setMode(double _v);
	auto getMode() const -> double;

	// Description:
	// Brightness of the image (only for cameras)..
	void setBrightness(double val);
	auto getBrightness() const -> double;
	// Description:
	// Contrast of the image (only for cameras).
	void setContrast(double val);
	auto getContrast() const -> double;
	// Description:
	// Saturation of the image (only for cameras).
	void setSaturation(double val);
	auto getSaturation() const -> double;
	// Description:
	//  Hue of the image (only for cameras).
	void setHue(double val);
	auto getHue() const -> double;
	// Description:
	// Gain of the image (only for cameras).
	void setGain(double val);
	auto getGain() const -> double;
	// Description:
	// Exposure (only for cameras).
	void setExposure(double val);
	auto getExposure() const -> double;
	// Description:
	// Temperature.
	void setTemperature(double val);
	auto getTemperature() const -> double;
	// Description:
	// Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
	void setRectification(double val);
	auto getRectification() const -> double;
	// Description:
	// Gamma.
	void setGamma(double val);
	auto getGamma() const -> double;
	// Description:
	// Set the value for camera sharpness. Specify zero in order to disable sharpness.
	void setSharpness(double val);
	auto getSharpness() const -> double;
	// Description:
	// Set 1 for auto-exposure and 0 for disabling it.
	void setAutoExposure(double val);
	auto getAutoExposure() const -> double;
	// Description:
	// Set 0 for disabling auto-white balance
	void setWhiteBalanceBlueU(double val);
	auto getWhiteBalanceBlueU() const -> double;
	// Description:
	// Set 0 for disabling auto-white balance
	void setWhiteBalanceRedV(double val);
	auto getWhiteBalanceRedV() const -> double;
	// Description:
	// Boolean flags indicating whether images should be converted to RGB.
	void setConvertToRGB(double val);
	auto getConvertToRGB() const -> double;

	// Description:
	// Set zoom.
	void setZoom(double val);
	auto getZoom() const -> double;
	// Description:
	// Set focus.
	void setFocus(double val);
	auto getFocus() const -> double;
	// Description:
	// Set pan.
	void setPan(double val);
	auto getPan() const -> double;
	// Description:
	// Set tilt.
	void setTilt(double val);
	auto getTilt() const -> double;
	// Description:
	// Set roll.
	void setRoll(double val);
	auto getRoll() const -> double;
	// Description:
	// The ISO speed of the camera (note: only supported by DC1394 v 2.x backend currently)
	void setIsoSpeed(double val);
	auto getIsoSpeed() const -> double;
	// Description:
	// Set back light.
	void setBackLight(double val);
	auto getBackLight() const -> double;
	// Description:
	// Set trigger.
	void setTrigger(double val);
	auto getTrigger() const -> double;
	// Description:
	// Set trigger delay.
	void setTriggerDelay(double val);
	auto getTriggerDelay() const -> double;

protected:
	// Description:
	// Overridden function to grab frames from the camera device or the video file.
	// If the video file path is specified, then this function will try to grab frames from the video file,
	// otherwise capturing from camera device will be ON. 
	// In order to grab from the camera device, the video file path
	// should be empty, like setVideoFile("");
	auto grab(cv::Mat& _m_frame) -> bool override;

	cv::VideoCapture m_cam;
	int m_videocapture_api;
	std::string m_filepath;
	std::atomic<bool> m_isrepeat;
};

}

#endif // fvkCameraThreadOpenCV_h__