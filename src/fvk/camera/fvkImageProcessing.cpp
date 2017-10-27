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

#include <fvk/camera/fvkImageProcessing.h>

using namespace R3D;

fvkImageProcessing::fvkImageProcessing() :
m_denoislevel(0),
m_denoismethod(DenoisingMethod::Gaussian),
m_sharplevel(0),
m_details(0),
m_smoothness(0),
m_pencilsketch(0),
m_stylization(0),
m_brigtness(0),
m_contrast(0),
m_colorcontrast(0),
m_saturation(0),
m_vibrance(0),
m_hue(0),
m_gamma(0),
m_sepia(0),
m_clip(0),
m_convertcolor(-1),
m_ndots(0),
m_isemboss(false),
m_rotangle(0),
m_isnegative(false),
m_zoomperc(100),
m_flip(FlipDirection::None),
m_isgray(false),
m_isfacetrack(false),
m_threshold(0),
m_equalizelimit(0)
{
}

fvkImageProcessing::~fvkImageProcessing()
{
}

void fvkImageProcessing::reset()
{
	m_denoislevel = 0;
	m_denoismethod = DenoisingMethod::Gaussian;
	m_sharplevel = 0;
	m_details = 0;
	m_smoothness = 0;
	m_pencilsketch = 0;
	m_stylization = 0;
	m_brigtness = 0;
	m_contrast = 0;
	m_colorcontrast = 0;
	m_saturation = 0;
	m_vibrance = 0;
	m_hue = 0;
	m_gamma = 0;
	m_sepia = 0;
	m_clip = 0;
	m_convertcolor = -1;
	m_ndots = 0;
	m_isemboss = false;
	m_rotangle = 0;
	m_isnegative = false;
	m_zoomperc = 100;
	m_flip = FlipDirection::None;
	m_isgray = false;
	m_isfacetrack = false;
	m_threshold = 0;
	m_equalizelimit = 0;
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

void fvkImageProcessing::setDenoisingFilter(cv::Mat& _img, int _value, fvkImageProcessing::DenoisingMethod _method)
{
	if (_img.empty() || _value < 2) return;

	if (_value % 2 != 0)
	{
		cv::Mat m(_img.size(), _img.type());
		if (_method == fvkImageProcessing::DenoisingMethod::Gaussian)
			cv::GaussianBlur(_img, m, cv::Size(_value, _value), 0, 0);
		else if (_method == fvkImageProcessing::DenoisingMethod::Blur)
			cv::blur(_img, m, cv::Size(_value, _value));
		else if (_method == fvkImageProcessing::DenoisingMethod::Median)
			cv::medianBlur(_img, m, _value);
		else if (_method == fvkImageProcessing::DenoisingMethod::Bilateral)
			cv::bilateralFilter(_img, m, _value, _value * 2, _value / 2);
		else if (_method == fvkImageProcessing::DenoisingMethod::NL_Mean)
		{
			if (_img.channels() == 3)
				cv::fastNlMeansDenoisingColored(_img, m, 3.0f, 3.0f, _value, 21);
			else
				cv::fastNlMeansDenoising(_img, m, 3.0f, _value, 21);
		}
		_img = m;
	}
}

void fvkImageProcessing::setWeightedFilter(cv::Mat& _img, int _value, double _alpha, double _beta)
{
	if (_img.empty() || _value == 0) return;

	cv::Mat m(_img.size(), _img.type());
	cv::GaussianBlur(_img, m, cv::Size(0, 0), static_cast<double>(_value));
	cv::addWeighted(_img, _alpha, m, _beta, 0, m);

	//cv::Mat kern = (cv::Mat_<char>(3, 3) <<
	//	0, -1, 0,
	//	-1, _value, -1,
	//	0, -1, 0);
	//cv::filter2D(_img, m, -1, kern);

	_img = m;
}

void fvkImageProcessing::setNonPhotorealisticFilter(cv::Mat& _img, int _value, float _sigma, fvkImageProcessing::Filters _filter)
{
	if (_img.empty() || _value == 0) return;
	if (_img.channels() != 3) return;

	cv::Mat m;
	if(_filter == fvkImageProcessing::Filters::Details)
		cv::detailEnhance(_img, m, static_cast<float>(_value), _sigma/*0.15f*/);
	if (_filter == fvkImageProcessing::Filters::Smoothing)
		cv::edgePreservingFilter(_img, m, cv::RECURS_FILTER, static_cast<float>(_value), _sigma/*0.1f*/);
	if (_filter == fvkImageProcessing::Filters::PencilSketch)
		cv::pencilSketch(_img, m, m, static_cast<float>(_value), _sigma/*0.07f*/, 0.03f);
	if (_filter == fvkImageProcessing::Filters::Stylization)
		cv::stylization(_img, m, static_cast<float>(_value), _sigma/*0.45f*/);
	_img = m;
}


void fvkImageProcessing::setBrightnessFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	int value = cvFloor(255.f * (static_cast<float>(_value) / 100.f));
	cv::Mat m = cv::Mat::zeros(_img.size(), _img.type());
	_img.convertTo(m, -1, 1.0, static_cast<double>(value));
	_img = m;

	// OR
	//
	//if (_img.channels() == 1)
	//{
	//	cv::Mat m(_img.size(), _img.type());
	//	for (int y = 0; y < _img.rows; y++)
	//		for (int x = 0; x < _img.cols; x++)
	//			m.at<uchar>(cv::Point(x, y)) = _img.at<uchar>(cv::Point(x, y)) + value;
	//	_img = m;
	//}
	//else if (_img.channels() == 3)
	//{
	//	cv::Mat m(_img.size(), _img.type());
	//	for (int y = 0; y < _img.rows; y++)
	//	{
	//		for (int x = 0; x < _img.cols; x++)
	//		{
	//			cv::Vec3i p = _img.at<cv::Vec3b>(cv::Point(x, y));
	//			p.val[0] += value;
	//			p.val[1] += value;
	//			p.val[2] += value;
	//			m.at<cv::Vec3b>(cv::Point(x, y)) = p;
	//		}
	//	}
	//	_img = m;
	//}

}

void fvkImageProcessing::setContrastFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	double value = std::pow(static_cast<double>(_value + 100) / 100.0, 2.0);
	cv::Mat m = cv::Mat::zeros(_img.size(), _img.type());
	_img.convertTo(m, -1, value, 0.0);
	_img = m;
}

void fvkImageProcessing::setColorContrastFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	float value = std::pow(static_cast<float>(_value + 100) / 100.f, 2.f);

	if (_img.channels() == 1)
	{
		cv::Mat m(_img.size(), _img.type());
		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				float p = static_cast<float>(_img.at<uchar>(cv::Point(x, y)));

				p /= 255.f;
				p -= 0.5f;
				p *= value;
				p += 0.5f;
				p *= 255.f;

				m.at<uchar>(cv::Point(x, y)) = static_cast<uchar>(p);
			}
		}
		_img = m;
	}
	else if (_img.channels() == 3)
	{
		cv::Mat m(_img.size(), _img.type());
		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				cv::Vec3f pixel = _img.at<cv::Vec3b>(cv::Point(x, y));

				pixel.val[0] /= 255.f;
				pixel.val[0] -= 0.5f;
				pixel.val[0] *= value;
				pixel.val[0] += 0.5f;
				pixel.val[0] *= 255.f;

				pixel.val[1] /= 255.f;
				pixel.val[1] -= 0.5f;
				pixel.val[1] *= value;
				pixel.val[1] += 0.5f;
				pixel.val[1] *= 255.f;

				pixel.val[2] /= 255.f;
				pixel.val[2] -= 0.5f;
				pixel.val[2] *= value;
				pixel.val[2] += 0.5f;
				pixel.val[2] *= 255.f;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		_img = m;
	}
	else if (_img.channels() == 4)
	{
		cv::Mat m(_img.size(), _img.type());
		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				cv::Vec4f pixel = _img.at<cv::Vec4b>(cv::Point(x, y));

				pixel.val[0] /= 255.f;
				pixel.val[0] -= 0.5f;
				pixel.val[0] *= value;
				pixel.val[0] += 0.5f;
				pixel.val[0] *= 255.f;

				pixel.val[1] /= 255.f;
				pixel.val[1] -= 0.5f;
				pixel.val[1] *= value;
				pixel.val[1] += 0.5f;
				pixel.val[1] *= 255.f;

				pixel.val[2] /= 255.f;
				pixel.val[2] -= 0.5f;
				pixel.val[2] *= value;
				pixel.val[2] += 0.5f;
				pixel.val[2] *= 255.f;

				m.at<cv::Vec4b>(cv::Point(x, y)) = pixel;
			}
		}
		_img = m;
	}
}

void fvkImageProcessing::setSaturationFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	float value = _value * -0.01f;

	if (_img.channels() == 3)
	{
		cv::Mat m(_img.size(), _img.type());
		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				cv::Vec3f pixel = _img.at<cv::Vec3b>(cv::Point(x, y));

				float maxi = std::max(std::max(pixel.val[0], pixel.val[1]), pixel.val[2]);
				if (pixel.val[0] != maxi)
					pixel.val[0] += (maxi - pixel.val[0]) * value;

				if (pixel.val[1] != maxi)
					pixel.val[1] += (maxi - pixel.val[1]) * value;

				if (pixel.val[2] != maxi)
					pixel.val[2] += (maxi - pixel.val[2]) * value;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		_img = m;
	}
}

void fvkImageProcessing::setVibranceFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	float value = _value * -1.0f;

	if (_img.channels() == 3)
	{
		cv::Mat m(_img.size(), _img.type());
		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				cv::Vec3f pixel = _img.at<cv::Vec3b>(cv::Point(x, y));

				float maxi = std::max(std::max(pixel.val[0], pixel.val[1]), pixel.val[2]);

				float avg = (pixel.val[0] + pixel.val[1] + pixel.val[2]) / 3.f;
				float amt = ((std::abs(maxi - avg) * 2.f / 255.f) * value) / 100.f;


				if (pixel.val[0] != maxi)
					pixel.val[0] += (maxi - pixel.val[0]) * amt;

				if (pixel.val[1] != maxi)
					pixel.val[1] += (maxi - pixel.val[1]) * amt;

				if (pixel.val[2] != maxi)
					pixel.val[2] += (maxi - pixel.val[2]) * amt;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		_img = m;
	}
}

void fvkImageProcessing::setHueFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	if (_img.channels() == 3)
	{
		cv::Mat m;
		cv::cvtColor(_img, m, cv::ColorConversionCodes::COLOR_BGR2HSV);	// BGR to HSV

		for (int y = 0; y < m.rows; y++)
		{
			for (int x = 0; x < m.cols; x++)
			{
				cv::Vec3i pixel = m.at<cv::Vec3b>(cv::Point(x, y));

				pixel.val[0] += _value;
				if (pixel.val[0] < 0) 
					pixel.val[0] = 0; 
				else if (pixel.val[0] > 255) 
					pixel.val[0] = 255;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		cv::cvtColor(m, m, cv::ColorConversionCodes::COLOR_HSV2BGR);	// HSV back to BGR
		_img = m;
	}
}

void fvkImageProcessing::setGammaFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	float value = 1.f - static_cast<float>(_value) / 100.f;

	if (_img.channels() == 3)
	{
		cv::Mat m(_img.size(), _img.type());
		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				cv::Vec3f pixel = _img.at<cv::Vec3b>(cv::Point(x, y));

				pixel.val[0] = std::pow(pixel.val[0] / 255.f, value) * 255.f;
				pixel.val[1] = std::pow(pixel.val[1] / 255.f, value) * 255.f;
				pixel.val[2] = std::pow(pixel.val[2] / 255.f, value) * 255.f;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		_img = m;
	}
}

void fvkImageProcessing::setSepiaFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	if (_img.channels() == 3)
	{
		double value = static_cast<double>(_value) / 100.0;
		cv::Mat m(_img.size(), _img.type());

		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				cv::Vec3d p = _img.at<cv::Vec3b>(cv::Point(x, y));

				p.val[2] = std::min(255.0, (p.val[2] * (1.0 - (0.607 * value))) + (p.val[1] * (0.769 * value)) + (p.val[0] * (0.189 * value)));
				p.val[1] = std::min(255.0, (p.val[2] * (0.349 * value)) + (p.val[1] * (1.0 - (0.314 * value))) + (p.val[0] * (0.168 * value)));
				p.val[0] = std::min(255.0, (p.val[2] * (0.272 * value)) + (p.val[1] * (0.534 * value)) + (p.val[0] * (1.0 - (0.869 * value))));

				m.at<cv::Vec3b>(cv::Point(x, y)) = p;
			}
		}
		_img = m;
	}
}

void fvkImageProcessing::setClipFilter(cv::Mat& _img, int _value)
{
	if (_img.empty() || _value == 0) return;

	float value = std::abs(static_cast<float>(_value)) * 2.55f;

	if (_img.channels() == 3)
	{
		cv::Mat m(_img.size(), _img.type());
		for (int y = 0; y < _img.rows; y++)
		{
			for (int x = 0; x < _img.cols; x++)
			{
				cv::Vec3f pixel = _img.at<cv::Vec3b>(cv::Point(x, y));

				if (pixel.val[0] > (255.f - value))
					pixel.val[0] = 255.f;
				else if (pixel.val[0] < value)
					pixel.val[0] = 0.f;

				if (pixel.val[1] > (255.f - value))
					pixel.val[1] = 255.f;
				else if (pixel.val[1] < value)
					pixel.val[1] = 0.f;

				if (pixel.val[2] > (255.f - value))
					pixel.val[2] = 255.f;
				else if (pixel.val[2] < value)
					pixel.val[2] = 0.f;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		_img = m;
	}
}
void fvkImageProcessing::setEqualizeFilter(cv::Mat& _img, double _cliplimit, cv::Size _tile_grid_size)
{
	if (_img.empty() || _cliplimit == 0) return;

	if (_img.channels() == 1)
	{
		cv::Mat m;
		cv::Ptr<cv::CLAHE> p = cv::createCLAHE(_cliplimit, _tile_grid_size); // adaptive histogram equalization
		p->apply(_img, m);
		_img = m;
	}
	else if (_img.channels() == 3)
	{
		cv::Mat m;
		std::vector<cv::Mat> channels(3);
		cv::cvtColor(_img, m, CV_BGR2YCrCb);	//change to YCrCb format
		cv::split(m, channels);					// split the image into channels
		cv::Ptr<cv::CLAHE> p = cv::createCLAHE(_cliplimit, _tile_grid_size);
		p->apply(channels[0], channels[0]);
		cv::merge(channels, m);					// merge 3 channels including the modified one
		cv::cvtColor(m, m, CV_YCrCb2BGR);		// change to BGR format
		_img = m;
	}
	else if (_img.channels() == 4)
	{
		cv::Mat m;
		std::vector<cv::Mat> channels(4);
		cv::split(_img, channels);

		std::vector<cv::Mat> channels3(3);
		channels3[0] = channels[0];
		channels3[1] = channels[1];
		channels3[2] = channels[2];
		cv::merge(channels3, m);

		cv::cvtColor(m, m, CV_BGR2YCrCb);
		cv::split(m, channels3);
		cv::Ptr<cv::CLAHE> p = cv::createCLAHE(_cliplimit, _tile_grid_size);
		p->apply(channels3[0], channels3[0]);
		cv::merge(channels3, m);
		cv::cvtColor(m, m, CV_YCrCb2BGR);
		
		cv::Mat dst(_img.size(), _img.type());
		for (int y = 0; y < m.rows; y++)
		{
			for (int x = 0; x < m.cols; x++)
			{
				cv::Vec3b pixel = m.at<cv::Vec3b>(cv::Point(x, y));
				cv::Vec4b p(pixel.val[0], pixel.val[1], pixel.val[2], channels[3].at<uchar>(cv::Point(x, y)));
				dst.at<cv::Vec4b>(cv::Point(x, y)) = p;

			}
		}

		_img = dst;
	}
}

void fvkImageProcessing::imageProcessing(cv::Mat& _frame)
{
	m_mutex.lock();

	if (m_isfacetrack)
		m_ft.detect(_frame, 5);

	if (m_denoislevel > 2)
		setDenoisingFilter(_frame, m_denoislevel, m_denoismethod);

	if (m_smoothness > 0)
		setNonPhotorealisticFilter(_frame, m_smoothness, 0.1f, fvkImageProcessing::Filters::Smoothing);

	if (m_equalizelimit > 0)
		setEqualizeFilter(_frame, m_equalizelimit, cv::Size(8, 8));

	if (m_sharplevel > 0)
		setWeightedFilter(_frame, m_sharplevel, 1.5, -0.5);

	if (m_details > 0)
		setNonPhotorealisticFilter(_frame, m_details, 0.02f, fvkImageProcessing::Filters::Details);

	if (m_pencilsketch > 0)
		setNonPhotorealisticFilter(_frame, m_pencilsketch, 0.1f, fvkImageProcessing::Filters::PencilSketch);

	if (m_stylization > 0)
		setNonPhotorealisticFilter(_frame, m_stylization, 0.45f, fvkImageProcessing::Filters::Stylization);

	if (m_brigtness != 0)
		setBrightnessFilter(_frame, m_brigtness);

	if (m_contrast != 0)
		setContrastFilter(_frame, m_contrast);

	if (m_colorcontrast != 0)
		setColorContrastFilter(_frame, m_colorcontrast);

	if (m_saturation != 0)
		setSaturationFilter(_frame, m_saturation);

	if (m_vibrance != 0)
		setVibranceFilter(_frame, m_vibrance);

	if (m_hue != 0)
		setHueFilter(_frame, m_hue);

	if (m_gamma != 0)
		setGammaFilter(_frame, m_gamma);

	if (m_sepia > 0)
		setSepiaFilter(_frame, m_sepia);

	if (m_clip > 0)
		setClipFilter(_frame, m_clip);

	if (m_isnegative)
	{
		cv::Mat m;
		cv::bitwise_not(_frame, m);
		_frame = m;
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

	if (m_ndots > 5)
	{
		if (_frame.channels() == 4)
			cv::cvtColor(_frame, _frame, cv::ColorConversionCodes::COLOR_BGRA2BGR);
		else if (_frame.channels() == 1)
			cv::cvtColor(_frame, _frame, cv::ColorConversionCodes::COLOR_GRAY2BGR);

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

	if (m_flip != FlipDirection::None)
	{
		cv::Mat m;
		if (m_flip == FlipDirection::Horizontal)
			cv::flip(_frame, m, 0);
		else if (m_flip == FlipDirection::Vertical)
			cv::flip(_frame, m, 1);
		else if (m_flip == FlipDirection::Both)
			cv::flip(_frame, m, -1);
		_frame = m;
	}

	if (m_zoomperc > 0 && m_zoomperc != 100)
	{
		cv::Size s = __resizeKeepAspectRatio(_frame.cols, _frame.rows, static_cast<int>(static_cast<float>(_frame.cols * (m_zoomperc / 100.f))), static_cast<int>(static_cast<float>(_frame.rows * (m_zoomperc / 100.f))));
		cv::Mat m = cv::Mat::zeros(s, _frame.type());
		cv::resize(_frame, m, s, 0, 0, cv::InterpolationFlags::INTER_LINEAR);
		_frame = m;
	}

	if (m_rotangle != 0)
	{
		cv::Point2d cen(static_cast<double>(_frame.cols) / 2.0, static_cast<double>(_frame.rows) / 2.0);
		cv::Mat rot_mat = cv::getRotationMatrix2D(cen, m_rotangle, 1.0);
		cv::Mat m;
		cv::warpAffine(_frame, m, rot_mat, _frame.size(), cv::InterpolationFlags::INTER_LINEAR);
		_frame = m;
	}

	if (m_convertcolor >= 0)
	{
		cv::Mat m;
		cv::cvtColor(_frame, m, m_convertcolor);
		_frame = m;
	}

	if (m_isgray)
	{
		cv::Mat m;
		if (_frame.channels() == 3)
		{
			cv::cvtColor(_frame, m, cv::ColorConversionCodes::COLOR_BGR2GRAY);
			_frame = m;
		}
		else if (_frame.channels() == 4)
		{
			cv::cvtColor(_frame, m, cv::ColorConversionCodes::COLOR_BGRA2GRAY);
			_frame = m;
		}
	}

	if (m_threshold > 0)
	{
		cv::Mat m;
		if (_frame.channels() == 3)
			cv::cvtColor(_frame, m, CV_BGR2GRAY);
		else if (_frame.channels() == 4)
			cv::cvtColor(_frame, m, CV_BGRA2GRAY);
		cv::GaussianBlur(m, m, cv::Size(5, 5), 0, 0);
		cv::threshold(m, m, 255 - m_threshold, 255, cv::THRESH_BINARY);
		_frame = m;
	}

	if(m_isfacetrack)
		cv::rectangle(_frame, m_ft.get().getRect(), cv::Vec3b(166, 154, 75));

	m_mutex.unlock();
}

void fvkImageProcessing::setDenoisingMethod(fvkImageProcessing::DenoisingMethod _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_denoismethod = _value;
}
fvkImageProcessing::DenoisingMethod fvkImageProcessing::getDenoisingMethod()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_denoismethod;
}
void fvkImageProcessing::setDenoisingLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_denoislevel = _value;
}
int fvkImageProcessing::getDenoisingLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_denoislevel;
}

void fvkImageProcessing::setSharpeningLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_sharplevel = _value;
}
int fvkImageProcessing::getSharpeningLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_sharplevel;
}

void fvkImageProcessing::setDetailLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_details = _value;
}
int fvkImageProcessing::getDetailLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_details;
}
void fvkImageProcessing::setSmoothness(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_smoothness = _value;
}
int fvkImageProcessing::getSmoothness()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_smoothness;
}
void fvkImageProcessing::setPencilSketchLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_pencilsketch = _value;
}
int fvkImageProcessing::getPencilSketchLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_pencilsketch;
}
void fvkImageProcessing::setStylizationLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_stylization = _value;
}
int fvkImageProcessing::getStylizationLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_stylization;
}

void fvkImageProcessing::setBrightness(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_brigtness = _value;
}
int fvkImageProcessing::getBrightness()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_brigtness;
}

void fvkImageProcessing::setContrast(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_contrast = _value;
}
int fvkImageProcessing::getContrast()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_contrast;
}

void fvkImageProcessing::setColorContrast(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_colorcontrast = _value;
}
int fvkImageProcessing::getColorContrast()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_colorcontrast;
}

void fvkImageProcessing::setSaturation(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_saturation = _value;
}
int fvkImageProcessing::getSaturation()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_saturation;
}

void fvkImageProcessing::setVibrance(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_vibrance = _value;
}
int fvkImageProcessing::getVibrance()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_vibrance;
}

void fvkImageProcessing::setHue(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_hue = _value;
}
int fvkImageProcessing::getHue()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_hue;
}

void fvkImageProcessing::setGamma(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_gamma = _value;
}
int fvkImageProcessing::getGamma()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_gamma;
}

void fvkImageProcessing::setSepia(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_sepia = _value;
}
int fvkImageProcessing::getSepia()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_sepia;
}

void fvkImageProcessing::setClip(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_clip = _value;
}
int fvkImageProcessing::getClip()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_clip;
}

void fvkImageProcessing::setNegativeModeEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isnegative = _value;
}
bool fvkImageProcessing::isNegativeModeEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isnegative;
}

void fvkImageProcessing::setLightEmbossEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isemboss = _value;
}
bool fvkImageProcessing::isLightEmbossEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isemboss;
}

void fvkImageProcessing::setDotPatternLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_ndots = _value;
}
int fvkImageProcessing::getDotPatternLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_ndots;
}

void fvkImageProcessing::setFlipDirection(FlipDirection _d)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_flip = _d;
}
fvkImageProcessing::FlipDirection fvkImageProcessing::getFlipDirection()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_flip;
}

void fvkImageProcessing::setZoomLevel(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_zoomperc = _value;
}
int fvkImageProcessing::getZoomLevel()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_zoomperc;
}

void fvkImageProcessing::setRotationAngle(double _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_rotangle = _value;
}
double fvkImageProcessing::getRotationAngle()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_rotangle;
}

void fvkImageProcessing::setConvertColor(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_convertcolor = _value;
}
int fvkImageProcessing::getConvertColor()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_convertcolor;
}

void fvkImageProcessing::setGrayScaleEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isgray = _value;
}
bool fvkImageProcessing::isGrayScaleEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isgray;
}

void fvkImageProcessing::setThresholdValue(int _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_threshold = _value;
}
int fvkImageProcessing::getThresholdValue()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_threshold;
}
void fvkImageProcessing::setEqualizeClipLimit(double _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_equalizelimit = _value;
}
double fvkImageProcessing::getEqualizeClipLimit()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_equalizelimit;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool fvkImageProcessing::loadCascadeClassifier(const std::string& _filename)
{
	return m_ft.loadCascadeClassifier(_filename);
}
void fvkImageProcessing::setFaceDetectionEnabled(bool _value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isfacetrack = _value;
}
bool fvkImageProcessing::isFaceDetectionEnabled()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isfacetrack;
}