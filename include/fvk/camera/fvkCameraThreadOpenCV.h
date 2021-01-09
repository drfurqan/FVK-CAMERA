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

#include "fvkCameraExport.h"

#include "fvkCameraThread.h"

namespace R3D
{

class FVK_CAMERA_EXPORT fvkCameraThreadOpenCV : public fvkCameraThread
{

public:
	// Description:
	// Default constructor that creates a camera thread for continuous streaming.
	// device_index is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// frame_size is the desired width and height of camera frame.
	// Specifying Size(-1, -1) will do the auto-selection for the captured frame size,
	// normally it enables the 640x480 resolution on most of web cams.
	// buffer is the semaphore buffer to synchronizer the processing thread with this camera thread.
	fvkCameraThreadOpenCV(const int device_index, const cv::Size& frame_size, const int api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY), fvkSemaphoreBuffer<cv::Mat>* buffer = nullptr);
	// Description:
	// Default constructor to start the given video file.
	// buffer is the semaphore to synchronizer the processing thread with this thread.
	// video_file is the absolute path of the video file.
	// If width and height is specified, then this will become the video frame resolution.
	// cv::Size(-1, -1) will do the auto-selection of the resolution, normally it enable the 640x480 resolution.
	// api = cv::VideoCaptureAPIs::CAP_ANY is the preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	fvkCameraThreadOpenCV(const std::string& video_file, const cv::Size& frame_size, const int api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY), fvkSemaphoreBuffer<cv::Mat>* buffer = nullptr);
	// Description:
	// Default destructor that stops the threads and closes the camera device.
	virtual ~fvkCameraThreadOpenCV();

	// Description:
	// Function to connect the camera device or video file.
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
	// Set true if you want to restart or repeat the video when it finishes.
	// This function is only for videos.
	// Default in true.
	void repeat(const bool b);
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
	void setAPI(const int api) { m_videocapture_api = api; }
	// Description:
	// Function to get the Video Capture preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	auto getAPI() const { return m_videocapture_api; }

	// Description:
	// Function to set the video file location like "D:\\face\\video.avi".
	// If the video file is specified before the calling connect() function, then the specified video
	// will be played, rather than camera device.
	// This function is only for videos.
	void setVideoFileLocation(const std::string& filename) { m_filepath = filename; }
	// Description:
	// Function to get the video file path.
	// This function is only for videos.
	auto getVideoFileLocation() const { return m_filepath; }

	/************************************************************************/
	/* Camera properties                                                    */
	/************************************************************************/
	// Description:
	// Calling this function opens a camera configuration dialog, if supported by the camera driver.
	auto openConfigurationDialog() -> bool;

	// Description:
	// Function to get the current camera resolution.
	auto getActiveFrameSize() const-> cv::Size;

	// Description:
	// Current position of the video file in milliseconds or video capture timestamp.
	auto setMsec(double v) -> bool;
	auto getMsec() const -> double;
	// Description:
	// 0-based index of the frame to be decoded/captured next.
	auto setPosFrames(double v) -> bool;
	auto getPosFrames() const -> double;
	// Description:
	// Relative position of the video file: 0 - start of the film, 1 - end of the film.
	auto setAviRatio(double v) -> bool;
	auto getAviRatio() const -> double;
	// Description:
	// desired camera frame rate.
	auto setFps(double v) -> bool;
	auto getFps() const -> double;
	// Description:
	// Function to set a 4-character code of codec.
	auto setFourCC(double v) -> bool;
	// Description:
	// Function that returns the 4-character code that identify a video codec.
	auto getFourCC() const -> std::string;
	// Description:
	// Number of frames in the video file.
	auto setFrameCount(double v) -> bool;
	auto getFrameCount() const -> double;
	// Description:
	// Format of the Mat objects returned by retrieve().
	auto setFormat(double v) -> bool;
	auto getFormat() const -> double;
	// Description:
	// Backend-specific value indicating the current capture mode.
	auto setMode(double v) -> bool;
	auto getMode() const -> double;

	// Description:
	// Brightness of the image (only for cameras)..
	auto setBrightness(double val) -> bool;
	auto getBrightness() const -> double;
	// Description:
	// Contrast of the image (only for cameras).
	auto setContrast(double val) -> bool;
	auto getContrast() const -> double;
	// Description:
	// Saturation of the image (only for cameras).
	auto setSaturation(double val) -> bool;
	auto getSaturation() const -> double;
	// Description:
	//  Hue of the image (only for cameras).
	auto setHue(double val) -> bool;
	auto getHue() const -> double;
	// Description:
	// Gain of the image (only for cameras).
	auto setGain(double val) -> bool;
	auto getGain() const -> double;
	// Description:
	// Exposure (only for cameras).
	auto setExposure(double val) -> bool;
	auto getExposure() const -> double;
	// Description:
	// Temperature.
	auto setTemperature(double val) -> bool;
	auto getTemperature() const -> double;
	// Description:
	// Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
	auto setRectification(double val) -> bool;
	auto getRectification() const -> double;
	// Description:
	// Gamma.
	auto setGamma(double val) -> bool;
	auto getGamma() const -> double;
	// Description:
	// Set the value for camera sharpness. Specify zero in order to disable sharpness.
	auto setSharpness(double val) -> bool;
	auto getSharpness() const -> double;
	// Description:
	// Set 1 for auto-exposure and 0 for disabling it.
	auto setAutoExposure(double val) -> bool;
	auto getAutoExposure() const -> double;
	// Description:
	// Set 0 for disabling auto-white balance
	auto setWhiteBalanceBlueU(double val) -> bool;
	auto getWhiteBalanceBlueU() const -> double;
	// Description:
	// Set 0 for disabling auto-white balance
	auto setWhiteBalanceRedV(double val) -> bool;
	auto getWhiteBalanceRedV() const -> double;
	// Description:
	// Boolean flags indicating whether images should be converted to RGB.
	auto setConvertToRGB(double val) -> bool;
	auto getConvertToRGB() const -> double;

	// Description:
	// Set zoom.
	auto setZoom(double val) -> bool;
	auto getZoom() const -> double;
	// Description:
	// Set focus.
	auto setFocus(double val) -> bool;
	auto getFocus() const -> double;
	// Description:
	// Set pan.
	auto setPan(double val) -> bool;
	auto getPan() const -> double;
	// Description:
	// Set tilt.
	auto setTilt(double val) -> bool;
	auto getTilt() const -> double;
	// Description:
	// Set roll.
	auto setRoll(double val) -> bool;
	auto getRoll() const -> double;
	// Description:
	// The ISO speed of the camera (note: only supported by DC1394 v 2.x backend currently)
	auto setIsoSpeed(double val) -> bool;
	auto getIsoSpeed() const -> double;
	// Description:
	// Set back light.
	auto setBackLight(double val) -> bool;
	auto getBackLight() const -> double;
	// Description:
	// Set trigger.
	auto setTrigger(double val) -> bool;
	auto getTrigger() const -> double;
	// Description:
	// Set trigger delay.
	auto setTriggerDelay(double val) -> bool;
	auto getTriggerDelay() const -> double;

protected:
	// Description:
	// Function to connect the camera device.
	// device_index is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// It returns true on success.
	auto open(const int device_index) -> bool override;
	// Description:
	// Function to connect the video file.
	// file_name is the name of the opened video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// If the video file is specified before the calling open() function, then the specified video
	// will be played, rather than camera device.
	// api is the preferred API for a capture object. for more info see (cv::VideoCaptureAPIs).
	// It returns true on success.
	virtual auto open(const std::string& file_name) -> bool;

	// Description:
	// Overridden function to grab frames from the camera device or the video file.
	// If the video file path is specified, then this function will try to grab frames from the video file,
	// otherwise capturing from camera device will be ON. 
	// In order to grab from the camera device, the video file path
	// should be empty, like setVideoFile("");
	auto grab(cv::Mat& m_frame) -> bool override;

	cv::VideoCapture m_cam;
	int m_videocapture_api;
	std::string m_filepath;
	std::atomic<bool> m_isrepeat;
};

}

#endif // fvkCameraThreadOpenCV_h__