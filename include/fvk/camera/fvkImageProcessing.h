#pragma once
#ifndef fvkImageProcessing_h__
#define fvkImageProcessing_h__

/*********************************************************************************
created:	2017/02/02   10:22AM
filename: 	fvkImageProcessing.h
file base:	fvkImageProcessing
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class to do some basic image processing.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkFaceDetector.h"

#include "opencv2/opencv.hpp"
#include <mutex>

namespace R3D
{

class FVK_CAMERA_EXPORT fvkImageProcessing
{
public:
	// Description:
	// Default constructor.
	//  _device is the id of the opened video capturing device (i.e. a camera index).
	//  If there is a single camera connected, just pass 0.
	//  In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// If _width and _height is specified, then this will become the camera frame resolution.
	fvkImageProcessing();
	// Description:
	// Destructor that destroys the data.
	virtual ~fvkImageProcessing();

	// Description:
	// Function to reset all the parameters of this object.
	void reset();

	// Description:
	// Denoising/smoothing methods. 
	enum class DenoisingMethod
	{
		Gaussian = 0,
		Blur,
		Median,
		Bilateral,
		NL_Mean		// Non-local Means Denoising algorithm
	};

	// Description:
	// Function to set denoising/smoothing method.
	// Default value is DenoisingMethod::Bilateral.
	void setDenoisingMethod(fvkImageProcessing::DenoisingMethod value);
	// Description:
	// Function to get denoising/smoothing method.
	// Default value is DenoisingMethod::Gaussian.
	auto getDenoisingMethod() -> fvkImageProcessing::DenoisingMethod;
	// Description:
	// Function to set denoising/smoothing level/kernel.
	// Default value is 0.
	// Specified denoising level must be an odd number, starting from 3.
	void setDenoisingLevel(int value);
	// Description:
	// Function to get denoising/smoothing level/kernel.
	auto getDenoisingLevel() -> int;

	// Description:
	// Function to set sharpness to the image.
	// Default value is 0.
	void setSharpeningLevel(int value);
	// Description:
	// Function to get sharpness.
	// Default value is 0.
	auto getSharpeningLevel() -> int;

	// Description:
	// Function to enhance the detail of the image.
	// _value should be between 0 and 200.
	// Default value is 0.
	void setDetailLevel(int value);
	// Description:
	// Function to get the detail value.
	// Default value is 0.
	auto getDetailLevel() -> int;

	// Description:
	// Function to adjust the smoothness of the image.
	// _value should be between 0 and 200.
	// Default value is 0.
	void setSmoothness(int value);
	// Description:
	// Function to get the smoothness value.
	// Default value is 0.
	auto getSmoothness() -> int;

	// Description:
	// Function to adjust the pencil sketch of the image.
	// _value should be between 0 and 200.
	// Default value is 0.
	void setPencilSketchLevel(int value);
	// Description:
	// Function to get the pencil sketch value.
	// Default value is 0.
	auto getPencilSketchLevel() -> int;

	// Description:
	// Function to adjust the stylization of the image.
	// _value should be between 0 and 200.
	// Default value is 0.
	void setStylizationLevel(int value);
	// Description:
	// Function to get the stylization value.
	// Default value is 0.
	auto getStylizationLevel() -> int;

	// Description:
	// Function to adjust the brightness of the image.
	// _value should be between -100 and 100.
	// Default value is 0.
	void setBrightness(int value);
	// Description:
	// Function to get the brightness value.
	// Default value is 0.
	auto getBrightness() -> int;

	// Description:
	// Function to adjust the overall contrast of the image.
	// _value should be between -100 and 100.
	// Default value is 0.
	void setContrast(int value);
	// Description:
	// Function to get contrast value.
	// Default value is 0.
	auto getContrast() -> int;

	// Description:
	// Function to adjust the color contrast of the image.
	// _value should be between -100 and 100.
	// Default value is 0.
	void setColorContrast(int value);
	// Description:
	// Function to get the color contrast value.
	// Default value is 0.
	auto getColorContrast() -> int;

	// Description:
	// Function to adjust the color saturation of the image.
	// _value should be between -100 and 100.
	// _value less than 0 will desaturate the image while values greater than 0 will saturate it.
	void setSaturation(int value);
	// Description:
	// Function to get the color saturation value.
	// Default value is 0.
	auto getSaturation() -> int;

	// Description:
	// Function to adjust the vibrancy of the image.
	// _value should be between -100 and 100.
	// _value less than 0 will desaturate the image while values greater than 0 will saturate it.
	// Default value is 0.
	void setVibrance(int _value);
	// Description:
	// Function to adjust the vibrancy value.
	// Default value is 0.
	auto getVibrance() -> int;

	// Description:
	// Function to adjust the hue of the image.
	// _value should be between 0 and 100.
	// Default value is 0.
	void setHue(int value);
	// Description:
	// Function to get the hue value.
	// Default value is 0.
	auto getHue() -> int;

	// Description:
	// Function to adjust the gamma of the image.
	// _value should be between -100 and 100.
	// Default value is 0.
	void setGamma(int value);
	// Description:
	// Function to get the gamma value.
	// Default value is 0.
	auto getGamma() -> int;
	// Description:
	// Function to adjust the exposure of the image.
	// _value should be between -100 and 100.
	// Default value is 0.
	void setExposure(int value);
	// Description:
	// Function to get the exposure value.
	// Default value is 0.
	auto getExposure() -> int;

	// Description:
	// Function to adjust the sepia filter to the image.
	// _value should be between 0 and 100.
	// Default value is 0.
	void setSepia(int value);
	// Description:
	// Function to get the sepia filter value.
	// Default value is 0.
	auto getSepia() -> int;

	// Description:
	// Function to specify clip value that clips a color to max values 
	// when it falls outside of the specified range.
	// _value should be between 0 and 100.
	// Default value is 0.
	void setClip(int value);
	// Description:
	// Function to get the clip value.
	// Default value is 0.
	auto getClip() -> int;

	// Description:
	// Function to enable the negative mode.
	// Default value is false.
	void setNegativeModeEnabled(bool value);
	// Description:
	// Function that returns true if the negative mode is enabled.
	auto isNegativeModeEnabled() -> bool;

	// Description:
	// Function to enable light emboss.
	// Default value is false.
	void setLightEmbossEnabled(bool value);
	// Description:
	// Function that returns true if light emboss is enabled.
	auto isLightEmbossEnabled() -> bool;

	// Description:
	// Function to set dot pattern level to draw dots.
	// Specified level must be greater than 5.
	// Default value is 0.
	void setDotPatternLevel(int value);
	// Description:
	// Function to get dot pattern level.
	auto getDotPatternLevel() -> int;

	// Description:
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
	void setFlipDirection(FlipDirection d);
	// Description:
	// Function to get the flip direction.
	auto getFlipDirection() -> FlipDirection;

	// Description:
	// Function to set zoom in percentage.
	// Default value is 100%, meaning original size.
	// Specified level must be greater than 0.
	void setZoomLevel(int percentage);
	// Description:
	// Function to get zoom in percentage.
	auto getZoomLevel() -> int;

	// Description:
	// Function to set rotation angle in degrees.
	// Default value is 0.
	void setRotationAngle(double angle);
	// Description:
	// Function to get rotation angle in degrees.
	auto getRotationAngle() -> double;

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
	void setConvertColor(int value);
	// Description:
	// Function to get color conversion code.
	auto getConvertColor() -> int;

	// Description:
	// Function to turn gray-scale or black-and-white mode ON/OFF.
	void setGrayScaleEnabled(bool value);
	// Description:
	// Function that returns true if the gray-scale or black-and-white mode is ON.
	auto isGrayScaleEnabled() -> bool;

	// Description:
	// Function to specify the binary threshold value.
	// _value should be between 0 and 255.
	// Default value is 0.
	void setThresholdValue(int value);
	// Description:
	// Function to get the binary threshold value.
	auto getThresholdValue() -> int;

	// Description:
	// Function to specify the equalize clip limit value.
	// _value should be between 0 and 100.
	// Default value is 0.
	void setEqualizeClipLimit(double value);
	// Description:
	// Function to get the equalize clip limit value.
	auto getEqualizeClipLimit() -> double;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	// Description:
	// Function to load a classifier from a file.
	// filename Name of the file from which the classifier is loaded.
	// It returns true on success.
	auto loadCascadeClassifier(const std::string& fileName) -> bool;
	// Description:
	// Function to turn ON/OFF face tracking.
	// Default value is false.
	void setFaceDetectionEnabled(bool value);
	// Description:
	// Function that returns true if face tracking is enabled.
	auto isFaceDetectionEnabled() -> bool;
	// Description:
	// Function to get a reference to face detector.
	auto& getSimpleFaceDetector() { return m_ft; }

	// Description:
	// Function to clips a color to max values when it falls outside of the specified range.
	// _value should be between 0 and 100.
	static void setDenoisingFilter(cv::Mat& img, int value, fvkImageProcessing::DenoisingMethod method);
	// Description:
	// Function to apply various kinds of image processing filters such as sharpen an image.
	// _value should be between 0 and 100.
	static void setWeightedFilter(cv::Mat& img, int value, double alpha = 1.5, double beta = -0.5);
	// Description:
	// Flip directions.
	enum class Filters
	{
		Details = 0,
		Smoothing,
		PencilSketch,
		Stylization,
	};
	// Description:
	// Function to do non-photorealistic rendering on the given image. 
	// _value should be between 0 and 200.
	// _sigma should be between 0 and 1.
	static void setNonPhotorealisticFilter(cv::Mat& img, int value, float sigma, fvkImageProcessing::Filters filter);
	// Description:
	// Function to adjust the brightness of the image.
	// _value should be between -100 and 100.
	// _value less than 0 will darken the image while values greater than 0 will brighten.
	static void setBrightnessFilter(cv::Mat& img, int value);
	// Description:
	// Function to adjust the overall contrast of the image.
	// _value should be between -100 and 100.
	// _value less than 0 will darken the image while values greater than 0 will brighten.
	static void setContrastFilter(cv::Mat& img, int value);
	// Description:
	// Function to adjust the color contrast of the image.
	// _value should be between -100 and 100.
	// _value less than 0 will decrees the contrast while values greater than 0 will increase it.
	static void setColorContrastFilter(cv::Mat& img, int value);
	// Description:
	// Function to adjust the color saturation of the image.
	// _value should be between -100 and 100.
	// _value less than 0 will desaturate the image while values greater than 0 will saturate it.
	static void setSaturationFilter(cv::Mat& _img, int _value);
	// Description:
	// Function to adjust the vibrancy of the image.
	// _value should be between -100 and 100.
	// _value less than 0 will desaturate the image while values greater than 0 will saturate it.
	static void setVibranceFilter(cv::Mat& img, int value);
	// Description:
	// Function to adjust the hue of the image.
	// _value should be between 0 and 100.
	static void setHueFilter(cv::Mat& img, int value);
	// Description:
	// Function to adjust the gamma of the image.
	// gamma value range should be [-100 ~ 100].
	static void setGammaFilter(cv::Mat& img, int value);
	// Description:
	// Function to adjust the exposure of the image.
	// exposure value range should be [-100 ~ 100].
	static void setExposureFilter(cv::Mat& img, int value);
	// Description:
	// Function to adjust the sepia filter to the image.
	// _value should be between 0 and 100.
	static void setSepiaFilter(cv::Mat& img, int value);
	// Description:
	// Function to clips a color to max values when it falls outside of the specified range.
	// _value should be between 0 and 100.
	static void setClipFilter(cv::Mat& img, int value);
	// Description:
	// Function to clips a color to max values when it falls outside of the specified range.
	// _value should be between 0 and 100.
	static void setEqualizeFilter(cv::Mat& img, double cliplimit, cv::Size tile_grid_size = cv::Size(8, 8));

	// Description:
	// Function to perform image processing algorithms.
	virtual void imageProcessing(cv::Mat& frame);

private:
	int m_denoislevel;
	DenoisingMethod m_denoismethod;
	int m_sharplevel;
	int m_smoothness;
	int m_details;
	int m_pencilsketch;
	int m_stylization;
	int m_brigtness;
	int m_contrast;
	int m_colorcontrast;
	int m_saturation;
	int m_vibrance;
	int m_hue;
	int m_gamma;
	int m_exposure;
	int m_sepia;
	int m_clip;
	int m_ndots;
	bool m_isemboss;
	double m_rotangle;
	bool m_isnegative;
	int m_zoomperc;
	FlipDirection m_flip;
	bool m_isgray;
	int m_convertcolor;
	int m_threshold;
	double m_equalizelimit;

	bool m_isfacetrack;
	fvkSimpleFaceDetector m_ft;

	std::mutex m_mutex;
};

}

#endif // fvkImageProcessing_h__