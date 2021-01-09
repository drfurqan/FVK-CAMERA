#pragma once
#ifndef fvkVideoWriter_h__
#define fvkVideoWriter_h__

/*********************************************************************************
created:	2014/05/12   11:01AM
modified:	2017/02/09   11:54PM
filename: 	fvkVideoWriter.h
file base:	fvkVideoWriter
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Thread safe class to create a video file using OpenCV video writer.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkCameraExport.h"

#include "opencv2/opencv.hpp"

namespace R3D
{

class FVK_CAMERA_EXPORT fvkVideoWriter
{

public:
	// Description:
	// Constructor to create a writer object.
	fvkVideoWriter();

	// Description:
	// Destructor that releases the writer.
	virtual ~fvkVideoWriter();

	// Description:
	// Initialize the video recorder by specifying the path of the file, width, height, fps, and codec type.
	//
	// SaveFileName = "C:\\video.avi".
	// width, height = width and height of the frame in pixels.
	// fps = desired frames per second.
	// selectcodec = -1 ===> If you pass minus one as argument, then a window 
	// will pop up at runtime that contains all the codec installed on your system and ask you to select the one to use.
	// If you want to select your own codec, then pass selectcodec=0 as zero. 
	// codec_c1, codec_c2, codec_c3, codec_c4 are the four character codec with which the video stream will be compressed.
	// Example. 
	// 'F','L','V','1'
	// 'I','4','2','0'
	// 'I','Y','U','V'
	// 'H','2','6','4'
	// 'x','2','6','4' => Compressed format.
	// 'M','J','P','G' => The four-character code for motion jpeg.
	// 'm','s','v','c' => Best way to try recorder feature is to use this microsoft codec, but rememder its an uncompressed format.
	// Note: There are so many codecs in circulation, but whichever codec you choose must be available on your machine.
	// For more information about codecs, visit http://www.fourcc.org/codecs.php
	// 
	// Returned value = -2 ===> Codec is invalid.
	// Returned value = -1 ===> Already initialized.
	// Returned value =  0 ===> Could not initialize.
	// Returned value =  1 ===> Successfully initialized.
	// 
	// Example:
	// fvkVideoWriter vr;
	// if(vr.open("C:\\video.avi", I0->width, I0->height, 25, 1, 1))
	// {
	//	 vr.addFrame(I0);
	//	 vr.addFrame(I1);
	//	 vr.done();
	// }
	virtual auto open() -> int;

	// Description:
	// Function that returns true if the video writer has been successfully initialized.
	auto isOpened() const -> bool;

	// Description:
	// Function to add a new image frame to the video file.
	virtual void addFrame(const cv::Mat& frame);

	// Description:
	// Function that stops video recoding and finalize the video file.
	// Without calling this function, video file won't be played.
	virtual void stop();

	// Description:
	// Function to set the api preference.
	// The _api parameter allows to specify API backends to use.
	// Can be used to enforce a specific reader implementation
	// if multiple are available: e.g., cv::CAP_FFMPEG or cv::CAP_INTEL_MFX.
	void setApi(int api) { m_api = api; }
	// Description:
	// Function to get the api preference.
	auto getApi() const { return m_api; }

	// Description:
	// Function to set the saved video file path.
	// The path folder should have writable permission, such as Pictures/Videos/Documents folder.
	// Example:
	// p->setOutputLocation("D:\\video.mp4");
	void setOutputLocation(const std::string& val) { m_file = val; }
	// Description:
	// Function to get the saved video file path.
	auto getOutputLocation() const { return m_file; }

	// Description:
	// Function to set the recorded video resolution i.e, width and height.
	void setSize(const cv::Size& s) { m_size = s; }
	// Description:
	// Function to get the recorded video resolution i.e, width and height.
	auto getSize() const { return m_size; }
	// Description:
	// Function to set the desired frames per second.
	void setFps(double fps) { m_fps = fps; }
	// Description:
	// Function to get the specified frames per second.
	auto getFps() const { return m_fps; }
	// Description:
	// If it's true, video will be recored with RGB colors.
	// Default value is true.
	void setColored(bool b) { m_iscolor = b; }
	// Description:
	// It returns true, if the video is recorded with RGB colors.
	auto isColored() const { return m_iscolor; }
	// Description:
	// If it's true, then a window will pop up at runtime that contains all the 
	// codec installed on your system and ask you to select the one to use.
	// Default value is false.
	void autoCodecSelection(bool b) { m_autocodec = b; }
	// Description:
	// It returns true, auto-codec selection is ON.
	auto autoCodecSelection() const { return m_autocodec; }
	// Description:
	// Function to set four character codec with which the video stream will be compressed.
	// Example. 
	// FLV1
	// I420
	// IYUV
	// H264
	// x264 => Compressed format.
	// MJPG => The four-character code for motion jpeg.
	// MSVC => an uncompressed Microsoft format.
	// Note: There are so many codecs in circulation, but whichever codec you choose must be available on your machine.
	// For more information about codecs, visit http://www.fourcc.org/codecs.php
	// Default value is H264.
	void setCodec(const std::string& codec) { m_codec = codec; }
	// Description:
	// Function to get four character codec with which the video stream will be compressed.
	auto getCodec() const { return m_codec; }

private:
	cv::VideoWriter m_writer;
	int m_api;
	std::string m_file;
	cv::Size m_size;
	double m_fps;
	bool m_iscolor;
	bool m_autocodec;
	std::string m_codec;
};

}


#endif // fvkVideoWriter_h__
