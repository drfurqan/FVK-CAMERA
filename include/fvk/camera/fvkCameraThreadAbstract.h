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
	bool open(int _device_index);
	// Description:
	// Function to connect the video file.
	// _avi_filename is the name of the opened video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// _api is the preferred API for a capture object.
	// It returns true on success.
	bool open(const std::string& _avi_filename, cv::VideoCaptureAPIs _api = cv::CAP_ANY);
	// Description:
	// Function to connect the camera.
	// If the video file is specified before the calling this function, then this method will try to 
	// open the video file, otherwise it will open the camera device.
	// It returns true on success.
	bool open();
	
    // Description:
    // It returns true if the camera or video file is successfully connected.
	bool isOpened();
	// Description:
	// It returns true if the camera or video file is successfully disconnected.
	bool close();

	// Description:
	// Function that starts the thread.
	virtual void run() = 0;
	// Description:
	// Virtual function to be overridden to capture the frame. 
	virtual bool grab(cv::Mat& _m_frame) = 0;
	// Description:
	// Virtual function to be overridden to pause the camera capturing thread.
	virtual void pause(bool _b) = 0;
	// Description:
	// Virtual function to be overridden to get the pause status of the camera capturing thread.
	virtual bool pause() = 0;
	// Description:
	// Virtual function to be overridden to stop the camera capturing and it's thread.
	virtual void stop() = 0;

	// Description:
	//  Virtual function to be overridden to get the current grabbed frame.
	virtual cv::Mat getFrame() = 0;

	// Description:
	// Set true if you want to restart or repeat the video when it finishes. (only for videos)
	// Default in true.
	virtual void repeat(bool _b) { m_isrepeat = _b; }
	// It returns true if the repeat flag for the video is on. (only for videos)
	// Default in true.
	virtual bool repeat() { return m_isrepeat; }

	// Description:
	// Function to set the camera device id.
	//  _device is the id of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	void setDeviceId(int _id) { m_device_index = _id; }
	// Description:
	// Function to get the camera device id.
	int getDeviceId() const { return m_device_index; }

	// Description:
	// Function to set the frame delay which makes delay one thread for sometime.
	virtual void setFrameDelay(int _delay_msec) { m_frame_msec = _delay_msec; }
	// Description:
	// Function to get the frame delay.
	// Default delay for *.avi video file is 1000.0 / getFps(). (only for videos)
	virtual int getFrameDelay() const { return m_frame_msec; }

	// Description:
	// Function to set the video file path like "D:\\face\\video.avi". (only for videos)
	// If the video file is specified before the calling connect() function, then the specified video
	// will be played, rather than camera device.
	void setVideoFile(const std::string& _filename) { m_filepath = _filename; }
	// Description:
	// Function to get the video file path. (only for videos)
	std::string getVideoFile() const { return m_filepath; }

	/************************************************************************/
	/* Camera properties                                                    */
	/************************************************************************/
	// Description:
    // Call this function in order to open the camera driver config dialog.
    void openDriverConfigDialog();
	// Description:
	// Function to set the width of the captured frame.
	void setWidth(int _w) { m_width = _w; }
	// Description:
	// Function to get the width of the captured frame.
	int getWidth();
	// Description:
	// Function to set the height of the captured frame.
	void setHeight(int _h) { m_height = _h; }
	// Description:
	// Function to get the height of the captured frame.
	int getHeight();
	// Description:
	// Function to set the camera resolution (columns and rows).
	// Resolution should be specified before calling the open() function.
	void setResolution(cv::Size _res) { m_width = _res.width; m_height = _res.height; }
	// Description:
	// Function to get the current camera resolution.
	cv::Size getResolution();
	// Description:
	// Current position of the video file in milliseconds or video capture timestamp.
	void setMsec(double _v);
	double getMsec();
	// Description:
	// 0-based index of the frame to be decoded/captured next.
	void setPosFrames(double _v);
	double getPosFrames();
	// Description:
	// Relative position of the video file: 0 - start of the film, 1 - end of the film.
	void setAviRatio(double _v);
	double getAviRatio();
	// Description:
	// desired camera frame rate.
	void setFps(double _v);
	double getFps();
	// Description:
	// Function to set a 4-character code of codec.
	void setFourCC(double _v);
	// Description:
	// Function that returns the 4-character code that identify a video codec.
	std::string getFourCC();
	// Description:
	// Number of frames in the video file.
	void setFrameCount(double _v);
	double getFrameCount();
	// Description:
	// Format of the Mat objects returned by retrieve().
	void setFormat(double _v);
	double getFormat();
	// Description:
	// Backend-specific value indicating the current capture mode.
	void setMode(double _v);
	double getMode();

	// Description:
	// Brightness of the image (only for cameras)..
	void setBrightness(double val);
	double getBrightness();
	// Description:
	// Contrast of the image (only for cameras).
	void setContrast(double val);
	double getContrast();
	// Description:
	// Saturation of the image (only for cameras).
	void setSaturation(double val);
	double getSaturation();
	// Description:
	//  Hue of the image (only for cameras).
	void setHue(double val);
	double getHue();
	// Description:
	// Gain of the image (only for cameras).
	void setGain(double val);
	double getGain();
	// Description:
	// Exposure (only for cameras).
	void setExposure(double val);
	double getExposure();
	// Description:
	// Temperature.
	void setTemperature(double val);
	double getTemperature();
	// Description:
	// Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
	void setRectification(double val);
	double getRectification();
	// Description:
	// Gamma.
	void setGamma(double val);
	double getGamma();
	// Description:
	// Set the value for camera sharpness. Specify zero in order to disable sharpness.
    void setSharpness(double val);
	double getSharpness();
	// Description:
	// Set 1 for auto-exposure and 0 for disabling it.
    void setAutoExposure(double val);
	double getAutoExposure();
	// Description:
	// Set 0 for disabling auto-white balance
    void setWhiteBalanceBlueU(double val);
	double getWhiteBalanceBlueU();
	// Description:
	// Set 0 for disabling auto-white balance
	void setWhiteBalanceRedV(double val);
	double getWhiteBalanceRedV();
	// Description:
    // Boolean flags indicating whether images should be converted to RGB.
    void setConvertToRGB(double val);
	double getConvertToRGB();

	// Description:
	// Set zoom.
	void setZoom(double val);
	double getZoom();
	// Description:
	// Set focus.
	void setFocus(double val);
	double getFocus();
	// Description:
	// Set pan.
	void setPan(double val);
	double getPan();
	// Description:
	// Set tilt.
	void setTilt(double val);
	double getTilt();
	// Description:
	// Set roll.
	void setRoll(double val);
	double getRoll();
	// Description:
	// The ISO speed of the camera (note: only supported by DC1394 v 2.x backend currently)
	void setIsoSpeed(double val);
	double getIsoSpeed();
	// Description:
	// Set back light.
	void setBackLight(double val);
	double getBackLight();
	// Description:
	// Set trigger.
	void setTrigger(double val);
	double getTrigger();
	// Description:
	// Set trigger delay.
	void setTriggerDelay(double val);
	double getTriggerDelay();

protected:
	cv::VideoCapture p_cam;
	int m_device_index;
	int m_frame_msec;
	int m_width;
	int m_height;
	std::string m_filepath;
	bool m_isstop;
	bool m_ispause;
	bool m_isrepeat;
	cv::Mat m_frame;
};

}

#endif // fvkCameraThreadAbstract_h__
