/*********************************************************************************
created:	2017/02/02   10:22AM
filename: 	fvkCameraImageProcessing.cpp
file base:	fvkCameraImageProcessing
file ext:	cpp
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

#include <fvk/camera/fvkCameraImageProcessing.h>

using namespace R3D;

fvkCameraImageProcessing::fvkCameraImageProcessing() :
m_flip(FlipDirection::None),
m_isgray(false),
m_convertcolor(-1),
m_brigtness(0),
m_contrast(0),
m_saturation(0),
m_sharplevel(0),
m_denoislevel(0),
m_ndots(0),
m_isemboss(false),
m_rotangle(0),
m_isnegative(false),
m_zoomperc(100),
m_denoismethod(DenoisingMethod::Bilateral),
m_isfacetrack(false)
{
}

fvkCameraImageProcessing::~fvkCameraImageProcessing()
{
}

static cv::Size __resizeKeepAspectRatio(int _old_w, int _old_h, int _new_w, int _new_h)
{
	int _final_w = 0, _final_h = 0;
	double w2 = _new_h * (_old_w / static_cast<double>(_old_h));
	double h1 = _new_w * (_old_h / static_cast<double>(_old_w));
	if (h1 <= _new_h)
	{
		_final_w = _new_w;
		_final_h = static_cast<int>(h1);
	}
	else
	{
		_final_w = static_cast<int>(w2);
		_final_h = _new_h;
	}
	return cv::Size(_final_w, _final_h);
}

void fvkCameraImageProcessing::imageProcessing(cv::Mat& _frame)
{
	m_mutex.lock();

	if (m_isfacetrack)
	{
		m_ft.execute(_frame);
	}

	if (m_convertcolor >= 0)
	{
		cv::cvtColor(_frame, _frame, m_convertcolor);
	}

	if (m_isgray)
	{
		if (_frame.channels() == 3)
			cv::cvtColor(_frame, _frame, CV_BGR2GRAY);
		else if (_frame.channels() == 4)
			cv::cvtColor(_frame, _frame, CV_BGRA2GRAY);
	}

	if (m_brigtness > 0 || m_contrast > 0)
	{
		cv::Mat m = cv::Mat::zeros(_frame.size(), _frame.type());
		_frame.convertTo(m, -1, static_cast<double>(m_contrast / 100.0), static_cast<double>(m_brigtness));
		_frame = m;
	}

	if (m_saturation > 0)
	{
		cv::Mat m;
		cv::cvtColor(_frame, m, CV_BGR2HSV);	// BGR to HSV
		int idx = 1;
		for (int i = 0; i < m.rows; i++)
			for (int j = 0; j < m.cols; j++)
				m.at<cv::Vec3b>(i, j)[idx] += m_saturation;
		cv::cvtColor(m, _frame, CV_HSV2BGR);	// HSV back to BGR
	}

	if (m_denoislevel > 2)
	{
		if (m_denoismethod == fvkCameraImageProcessing::Gaussian)
			cv::GaussianBlur(_frame, _frame, cv::Size(m_denoislevel, m_denoislevel), 0, 0);
		else if (m_denoismethod == fvkCameraImageProcessing::Blur)
			cv::blur(_frame, _frame, cv::Size(m_denoislevel, m_denoislevel));
		else if (m_denoismethod == fvkCameraImageProcessing::Median)
			cv::medianBlur(_frame, _frame, m_denoislevel);
		else if (m_denoismethod == fvkCameraImageProcessing::Bilateral)
		{
			cv::Mat m(_frame.size(), _frame.type());
			cv::bilateralFilter(_frame, m, m_denoislevel, m_denoislevel * 2, m_denoislevel / 2);
			_frame = m;
		}
	}

	if (m_sharplevel > 0)
	{
		cv::Mat kern = (cv::Mat_<char>(3, 3) << 
			0, -1, 0,
			-1, 5, -1,
			0, -1, 0);
		cv::Mat m;
		cv::filter2D(_frame, m, _frame.depth(), kern);
		_frame = m;
	}

	if (m_ndots > 5)
	{
		if (_frame.channels() == 4)
			cv::cvtColor(_frame, _frame, CV_BGRA2BGR);
		else if (_frame.channels() == 1)
			cv::cvtColor(_frame, _frame, CV_GRAY2BGR);

		cv::Mat dst = cv::Mat::zeros(_frame.size(), CV_8UC3);
		cv::Mat cir = cv::Mat::zeros(_frame.size(), CV_8UC1);
		int bsize = m_ndots;

		for (int i = 0; i < _frame.rows; i += bsize)
		{
			for (int j = 0; j < _frame.cols; j += bsize)
			{
				cv::Rect rect = cv::Rect(j, i, bsize, bsize) & cv::Rect(0, 0, _frame.cols, _frame.rows);
				cv::Mat sub_dst(dst, rect);
				sub_dst.setTo(cv::mean(_frame(rect)));
				cv::circle(cir, cv::Point(j + bsize / 2, i + bsize / 2), bsize / 2 - 1, CV_RGB(255, 255, 255), -1, CV_AA);
			}
		}

		cv::Mat cir_32f;
		cir.convertTo(cir_32f, CV_32F);
		cv::normalize(cir_32f, cir_32f, 0, 1, cv::NORM_MINMAX);

		cv::Mat dst_32f;
		dst.convertTo(dst_32f, CV_32F);

		std::vector<cv::Mat> channels;
		cv::split(dst_32f, channels);
		for (size_t i = 0; i < channels.size(); ++i)
			channels[i] = channels[i].mul(cir_32f);

		cv::merge(channels, dst_32f);
		dst_32f.convertTo(dst, CV_8U);

		_frame = dst;
	}

	if (m_isemboss)
	{
		cv::Mat kern = (cv::Mat_<char>(3, 3) << 
			-1, -1, 0,
			-1, 0, 1,
			0, 1, 1);
		cv::Mat m;
		cv::filter2D(_frame, m, _frame.depth(), kern, cv::Point(-1, -1), 128);
		_frame = m;
	}

	if (m_isnegative)
	{
		cv::bitwise_not(_frame, _frame);
	}

	if (m_flip != FlipDirection::None)
	{
		if (m_flip == FlipDirection::Horizontal)
			cv::flip(_frame, _frame, 0);
		else if (m_flip == FlipDirection::Vertical)
			cv::flip(_frame, _frame, 1);
		else if (m_flip == FlipDirection::Both)
			cv::flip(_frame, _frame, -1);
	}

	if (m_zoomperc > 0 && m_zoomperc != 100)
	{
		cv::Size s = __resizeKeepAspectRatio(_frame.cols, _frame.rows, static_cast<int>(static_cast<float>(_frame.cols * (m_zoomperc / 100.f))), static_cast<int>(static_cast<float>(_frame.rows * (m_zoomperc / 100.f))));
		cv::Mat m = cv::Mat::zeros(s, _frame.type());
		cv::resize(_frame, m, s, 0, 0, CV_INTER_CUBIC);
		_frame = m;
	}

	if (m_rotangle != 0)
	{
		cv::Point2f cen;
		cen.x = static_cast<float>(_frame.cols) / 2.f;
		cen.y = static_cast<float>(_frame.rows) / 2.f;
		cv::Mat rot_mat = cv::getRotationMatrix2D(cen, m_rotangle, 1);
		cv::Mat rotated;
		cv::warpAffine(_frame, rotated, rot_mat, _frame.size(), cv::INTER_LINEAR);
		_frame = rotated;
	}

	//if (0)
	//{
	//	std::string fps = cv::format("Fps: %f", n);
	//	cv::putText(_frame, fps, cv::Point(10, 10), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
	//}
	m_mutex.unlock();
}

void fvkCameraImageProcessing::setFlipDirection(FlipDirection _d)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_flip = _d;
}
fvkCameraImageProcessing::FlipDirection fvkCameraImageProcessing::getFlipDirection()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_flip;
}

void fvkCameraImageProcessing::setGrayScaleEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isgray = _value;
}
bool fvkCameraImageProcessing::isGrayScaleEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isgray;
}

void fvkCameraImageProcessing::setConvertColor(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_convertcolor = _value;
}
int fvkCameraImageProcessing::getConvertColor()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_convertcolor;
}

void fvkCameraImageProcessing::setBrightness(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_brigtness = _value;
}
int fvkCameraImageProcessing::getBrightness()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_brigtness;
}

void fvkCameraImageProcessing::setContrast(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_contrast = _value;
}
int fvkCameraImageProcessing::getContrast()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_contrast;
}

void fvkCameraImageProcessing::setSaturation(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_saturation = _value;
}
int fvkCameraImageProcessing::getSaturation()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_saturation;
}

void fvkCameraImageProcessing::setSharpeningLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_sharplevel = _value;
}
int fvkCameraImageProcessing::getSharpeningLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_sharplevel;
}

void fvkCameraImageProcessing::setDenoisingMethod(DenoisingMethod _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_denoismethod = _value;
}
fvkCameraImageProcessing::DenoisingMethod fvkCameraImageProcessing::getDenoisingMethod()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_denoismethod;
}
void fvkCameraImageProcessing::setDenoisingLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	if (_value % 2 != 0 && _value > 2)
		m_denoislevel = _value;
}
int fvkCameraImageProcessing::getDenoisingLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_denoislevel;
}

void fvkCameraImageProcessing::setDotPatternLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_ndots = _value;
}
int fvkCameraImageProcessing::getDotPatternLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_ndots;
}

void fvkCameraImageProcessing::setLightEmbossEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isemboss = _value;
}
bool fvkCameraImageProcessing::isLightEmbossEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isemboss;
}

void fvkCameraImageProcessing::setRotationAngle(double _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_rotangle = _value;
}
double fvkCameraImageProcessing::getRotationAngle()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_rotangle;
}

void fvkCameraImageProcessing::setNegativeModeEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isnegative = _value;
}
bool fvkCameraImageProcessing::isNegativeModeEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isnegative;
}

void fvkCameraImageProcessing::setZoomLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_zoomperc = _value;
}
int fvkCameraImageProcessing::getZoomLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_zoomperc;
}

bool fvkCameraImageProcessing::loadCascadeClassifier(const std::string& _filename)
{
	return m_ft.loadCascadeClassifier(_filename);
}
void fvkCameraImageProcessing::setFaceTrackingEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isfacetrack = _value;
}
bool fvkCameraImageProcessing::isFaceTrackingEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isfacetrack;
}
void fvkCameraImageProcessing::setTrackedFaceColor(const cv::Vec3b& _rgb)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_ft.setTrackedFaceColor(_rgb);
}
cv::Vec3b fvkCameraImageProcessing::getTrackedFaceColor()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_ft.getTrackedFaceColor();
}
