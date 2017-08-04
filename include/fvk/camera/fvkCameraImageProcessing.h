#pragma once
#ifndef fvkCameraImageProcessing_h__
#define fvkCameraImageProcessing_h__

/*********************************************************************************
created:	2017/02/02   10:22AM
filename: 	fvkCameraImageProcessing.h
file base:	fvkCameraImageProcessing
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Class to do some basic image processing.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkFaceDetector.h"

#include <mutex>
#include "opencv2/opencv.hpp"

namespace R3D
{

class FVK_EXPORT fvkCameraImageProcessing
{
public:
	// Description:
	// Default constructor.
	//  _device is the id of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// If _width and _height is specified, then this will become the camera frame resolution.
	fvkCameraImageProcessing();
	virtual ~fvkCameraImageProcessing();

	// Description:
	// Function to perform image processing algorithms.
	virtual void imageProcessing(cv::Mat& _frame);

	// Description:
	// Function to turn gray-scale or black-and-white mode ON/OFF.
	void setGrayScaleEnabled(bool _value);
	// Description:
	// Function that returns true if the gray-scale or black-and-white mode is ON.
	bool isGrayScaleEnabled();

	// Description:
	// Function to set the conversion code in order to convert the captured
	// frame format in other formats.
	// See all constants for color conversion at
	// ...\opencv\include\opencv2\imgproc\types_c.h
	// For example:
	// Input color constant could be 
	// CV_BGRA2BGR
	// CV_BGR2HSV
	// CV_BGR2YCrCb
	// etc.
	// setConvertColor(CV_BGR2HSV);
	void setConvertColor(int _value);
	// Description:
	// Function to get the constract for color conversion.
	int getConvertColor();

	// Flip directions.
	enum class FlipDirection
	{
		None = 0,
		Horizontal,
		Vertical,
		Both
	};
	// Description:
	// Function to set the flip direction.
	// Default value is FlipDirection::None.
	void setFlipDirection(FlipDirection _d);
	// Description:
	// Function to get the flip direction.
	FlipDirection getFlipDirection();

	// Description:
	// Function to set brightness from [1~255].
	// Default value is 0.
	void setBrightness(int _value);
	// Description:
	// Function to get brightness.
	// Default value is 0.
	int getBrightness();

	// Description:
	// Function to set contrast from [1~100].
	// Default value is 0.
	void setContrast(int _value);
	// Description:
	// Function to get contrast.
	// Default value is 0.
	int getContrast();

	// Description:
	// Function to set saturation.
	// Default value is 0.
	void setSaturation(int _value);
	// Description:
	// Function to get saturation.
	// Default value is 0.
	int getSaturation();

	// Description:
	// Function to set sharpness.
	// Default value is 0.
	void setSharpeningLevel(int _value);
	// Description:
	// Function to get sharpness.
	// Default value is 0.
	int getSharpeningLevel();

	// Denoising/smoothing methods. 
	enum DenoisingMethod
	{
		Gaussian = 0,
		Blur,
		Median,
		Bilateral
	};

	// Description:
	// Function to set denoising/smoothing method.
	// Default value is DenoisingMethod::Bilateral.
	void setDenoisingMethod(DenoisingMethod _value);
	// Description:
	// Function to get denoising/smoothing method.
	// Default value is DenoisingMethod::Bilateral.
	DenoisingMethod getDenoisingMethod();
	// Description:
	// Function to set denoising/smoothing level/kernel.
	// Default value is 0.
	// Specified denoising level must be an odd number, starting from 3.
	void setDenoisingLevel(int _value);
	// Description:
	// Function to get denoising/smoothing level/kernel.
	int getDenoisingLevel();

	// Description:
	// Function to set dot pattern level to draw dots.
	// Default value is 0.
	// Specified level must be greater than 5.
	void setDotPatternLevel(int _value);
	// Description:
	// Function to get dot pattern level.
	int getDotPatternLevel();

	// Description:
	// Function to enable light emboss.
	// Default value is false.
	void setLightEmbossEnabled(bool _value);
	// Description:
	// Function that returns true if light emboss is enabled.
	bool isLightEmbossEnabled();

	// Description:
	// Function to set rotation angle in degrees.
	// Default value is 0.
	void setRotationAngle(double _angle);
	// Description:
	// Function to get rotation angle in degrees.
	double getRotationAngle();

	// Description:
	// Function to enable the negative mode.
	// Default value is false.
	void setNegativeModeEnabled(bool _value);
	// Description:
	// Function that returns true if the negative mode is enabled.
	bool isNegativeModeEnabled();

	// Description:
	// Function to set zoom in percentage.
	// Default value is 100%, meaning original size.
	// Specified level must be greater than 0.
	void setZoomLevel(int _percentage);
	// Description:
	// Function to get zoom in percentage.
	int getZoomLevel();

	// Description:
	// Function to load a classifier from a file.
	// filename Name of the file from which the classifier is loaded.
	// It returns true on success.
	bool loadCascadeClassifier(const std::string& _filename);
	// Description:
	// Function to turn ON/OFF face tracking.
	// Default value is false.
	void setFaceTrackingEnabled(bool _value);
	// Description:
	// Function that returns true if face tracking is enabled.
	bool isFaceTrackingEnabled();
	// Description:
	// Function to set RGB color to the rectangle of the tracked face.
	void setTrackedFaceColor(const cv::Vec3b& _rgb);
	// Description:
	// Function to get RGB color to the rectangle of the tracked face.
	cv::Vec3b getTrackedFaceColor();

private:
	FlipDirection m_flip;
	bool m_isgray;
	int m_convertcolor;
	int m_brigtness;
	int m_contrast;
	int m_saturation;
	int m_sharplevel;
	DenoisingMethod m_denoismethod;
	int m_denoislevel;
	int m_ndots;
	bool m_isemboss;
	double m_rotangle;
	bool m_isnegative;
	int m_zoomperc;

	bool m_isfacetrack;
	fvkFaceTracker m_ft;

	std::mutex m_mutex;
};

}

#endif // fvkCameraImageProcessing_h__
