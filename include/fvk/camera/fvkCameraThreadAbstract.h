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
	// Default constructor.
	fvkCameraThreadAbstract();
	// Description:
	// Default constructor.
	virtual ~fvkCameraThreadAbstract();

	// Description:
	// Function to connect the camera.
	//  _device_index is the index of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// It returns true on success.
	virtual auto open(int _device_index) -> bool;
	// Description:
	// Function to connect the video file.
	// _avi_filename is the name of the opened video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// _api is the preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	// It returns true on success.
	virtual auto open(const std::string& _avi_filename, int _api = cv::VideoCaptureAPIs::CAP_ANY) -> bool;
	// Description:
	// Function to connect the camera.
	// If the video file is specified before the calling this function, then this method will try to 
	// open the video file, otherwise it will open the camera device.
	// It returns true on success.
	virtual auto open() -> bool;
	
	// Description:
	// It returns true if the camera or video file is successfully connected.
	virtual auto isOpened() const -> bool;
	// Description:
	// It returns true if the camera or video file is successfully disconnected.
	virtual auto close() -> bool;

	// Description:
	// Virtual function to be overridden to capture the frame. 
	virtual bool grab(cv::Mat& _m_frame) = 0;

	// Description:
	//  Virtual function to be overridden to get the current grabbed frame.
	virtual cv::Mat getFrame() = 0;

	// Description:
	// Function to set the camera device index.
	//  _device is the id of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	void setDeviceIndex(int _index) { m_device_index = _index; }
	// Description:
	// Function to get the camera device index.
	auto getDeviceIndex() const { return m_device_index; }

	// Description:
	// Function to set the Video Capture preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	void setAPI(int _api) { m_videocapture_api = _api; }
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
	virtual void openDriverConfigDialog();
	// Description:
	// Function to set the width of the captured frame.
	void setWidth(int _w) { m_width = _w; }
	// Description:
	// Function to get the width of the captured frame.
	auto getWidth() const -> int;
	// Description:
	// Function to set the height of the captured frame.
	void setHeight(int _h) { m_height = _h; }
	// Description:
	// Function to get the height of the captured frame.
	virtual auto getHeight() const -> int;
	// Description:
	// Function to set the camera resolution (columns and rows).
	// Resolution should be specified before calling the open() function.
	void setResolution(cv::Size _res) { m_width = _res.width; m_height = _res.height; }
	// Description:
	// Function to get the current camera resolution.
	virtual auto getResolution() const -> cv::Size;
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
	cv::VideoCapture m_cam;
	int m_device_index;
	int m_width;
	int m_height;
	std::string m_filepath;
	int m_videocapture_api;
	cv::Mat m_frame;
};

}

#endif // fvkCameraThreadAbstract_h__
