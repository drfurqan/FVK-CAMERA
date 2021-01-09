/*********************************************************************************
created:	2017/02/02   10:22AM
filename: 	fvkImageProcessing.cpp
file base:	fvkImageProcessing
file ext:	cpp
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
	m_exposure(0),
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
	m_exposure = 0;
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


static cv::Size _resizeKeepAspectRatio(int old_w, int old_h, int new_w, int new_h)
{
	int final_w, final_h;
	auto w2 = new_h * (old_w / static_cast<double>(old_h));
	auto h1 = new_w * (old_h / static_cast<double>(old_w));
	if (h1 <= new_h)
	{
		final_w = new_w;
		final_h = static_cast<int>(h1);
	}
	else
	{
		final_w = static_cast<int>(w2);
		final_h = new_h;
	}
	return cv::Size(final_w, final_h);
}

void fvkImageProcessing::setDenoisingFilter(cv::Mat& img, int value, fvkImageProcessing::DenoisingMethod method)
{
	if (img.empty() || value < 2)
		return;

	if (value % 2 != 0)
	{
		cv::Mat m(img.size(), img.type());
		if (method == fvkImageProcessing::DenoisingMethod::Gaussian)
			cv::GaussianBlur(img, m, cv::Size(value, value), 0, 0);
		else if (method == fvkImageProcessing::DenoisingMethod::Blur)
			cv::blur(img, m, cv::Size(value, value));
		else if (method == fvkImageProcessing::DenoisingMethod::Median)
			cv::medianBlur(img, m, value);
		else if (method == fvkImageProcessing::DenoisingMethod::Bilateral)
			cv::bilateralFilter(img, m, value, value * 2, value / 2);
		else if (method == fvkImageProcessing::DenoisingMethod::NL_Mean)
		{
			if (img.channels() == 3)
				cv::fastNlMeansDenoisingColored(img, m, 3.0f, 3.0f, value, 21);
			else
				cv::fastNlMeansDenoising(img, m, 3.0f, value, 21);
		}
		img = m;
	}
}

void fvkImageProcessing::setWeightedFilter(cv::Mat& img, int value, double alpha, double beta)
{
	if (img.empty() || value == 0)
		return;

	cv::Mat m(img.size(), img.type());
	cv::GaussianBlur(img, m, cv::Size(0, 0), static_cast<double>(value));
	cv::addWeighted(img, alpha, m, beta, 0, m);

	//cv::Mat kern = (cv::Mat_<char>(3, 3) <<
	//	0, -1, 0,
	//	-1, value, -1,
	//	0, -1, 0);
	//cv::filter2D(img, m, -1, kern);

	img = m;
}

void fvkImageProcessing::setNonPhotorealisticFilter(cv::Mat& img, int value, float sigma, fvkImageProcessing::Filters filter)
{
	if (img.empty() || value == 0)
		return;

	if (img.channels() != 3)
		return;

	cv::Mat m;
	if(filter == fvkImageProcessing::Filters::Details)
		cv::detailEnhance(img, m, static_cast<float>(value), sigma/*0.15f*/);
	if (filter == fvkImageProcessing::Filters::Smoothing)
		cv::edgePreservingFilter(img, m, cv::RECURS_FILTER, static_cast<float>(value), sigma/*0.1f*/);
	if (filter == fvkImageProcessing::Filters::PencilSketch)
		cv::pencilSketch(img, m, m, static_cast<float>(value), sigma/*0.07f*/, 0.03f);
	if (filter == fvkImageProcessing::Filters::Stylization)
		cv::stylization(img, m, static_cast<float>(value), sigma/*0.45f*/);
	img = m;
}

void fvkImageProcessing::setBrightnessFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	auto v = cvFloor(255.f * (static_cast<float>(value) / 100.f));
	auto m = cv::Mat(cv::Mat::zeros(img.size(), img.type()));
	img.convertTo(m, -1, 1.0, static_cast<double>(v));
	img = m;

	// OR
	//
	//if (img.channels() == 1)
	//{
	//	cv::Mat m(img.size(), img.type());
	//	for (int y = 0; y < img.rows; y++)
	//		for (int x = 0; x < img.cols; x++)
	//			m.at<uchar>(cv::Point(x, y)) = img.at<uchar>(cv::Point(x, y)) + v;
	//	img = m;
	//}
	//else if (img.channels() == 3)
	//{
	//	cv::Mat m(img.size(), img.type());
	//	for (int y = 0; y < img.rows; y++)
	//	{
	//		for (int x = 0; x < img.cols; x++)
	//		{
	//			cv::Vec3i p = img.at<cv::Vec3b>(cv::Point(x, y));
	//			p.val[0] += value;
	//			p.val[1] += value;
	//			p.val[2] += value;
	//			m.at<cv::Vec3b>(cv::Point(x, y)) = p;
	//		}
	//	}
	//	img = m;
	//}

}
void fvkImageProcessing::setContrastFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0) return;

	auto v = std::pow(static_cast<double>(value + 100) / 100.0, 2.0);
	auto m = cv::Mat(cv::Mat::zeros(img.size(), img.type()));
	img.convertTo(m, -1, v, 0.0);
	img = m;
}
void fvkImageProcessing::setColorContrastFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	auto v = std::pow(static_cast<float>(value + 100) / 100.f, 2.f);

	if (img.channels() == 1)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				auto p = static_cast<float>(img.at<uchar>(cv::Point(x, y)));

				p /= 255.f;
				p -= 0.5f;
				p *= v;
				p += 0.5f;
				p *= 255.f;

				m.at<uchar>(cv::Point(x, y)) = static_cast<uchar>(p);
			}
		}
		img = m;
	}
	else if (img.channels() == 3)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec3f pixel = img.at<cv::Vec3b>(cv::Point(x, y));

				pixel.val[0] /= 255.f;
				pixel.val[0] -= 0.5f;
				pixel.val[0] *= v;
				pixel.val[0] += 0.5f;
				pixel.val[0] *= 255.f;

				pixel.val[1] /= 255.f;
				pixel.val[1] -= 0.5f;
				pixel.val[1] *= v;
				pixel.val[1] += 0.5f;
				pixel.val[1] *= 255.f;

				pixel.val[2] /= 255.f;
				pixel.val[2] -= 0.5f;
				pixel.val[2] *= v;
				pixel.val[2] += 0.5f;
				pixel.val[2] *= 255.f;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec4f pixel = img.at<cv::Vec4b>(cv::Point(x, y));

				pixel.val[0] /= 255.f;
				pixel.val[0] -= 0.5f;
				pixel.val[0] *= v;
				pixel.val[0] += 0.5f;
				pixel.val[0] *= 255.f;

				pixel.val[1] /= 255.f;
				pixel.val[1] -= 0.5f;
				pixel.val[1] *= v;
				pixel.val[1] += 0.5f;
				pixel.val[1] *= 255.f;

				pixel.val[2] /= 255.f;
				pixel.val[2] -= 0.5f;
				pixel.val[2] *= v;
				pixel.val[2] += 0.5f;
				pixel.val[2] *= 255.f;

				m.at<cv::Vec4b>(cv::Point(x, y)) = pixel;
			}
		}
		img = m;
	}
}
void fvkImageProcessing::setExposureFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	const auto v = static_cast<float>(value) / 50.f;
	const auto exposureFactor = std::pow(2.0f, v);

	if (img.channels() == 1)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				auto p = img.at<uchar>(y, x) / 255.f;
				p = (((p > 1.0f ? exposureFactor : p < 0.0f ? 0.0f : exposureFactor * p) - 0.5f) * 1.0) + 0.5f;
				p = p > 1.0f ? 255.0f : p < 0.0f ? 0.0f : 255.0f * p;
				m.at<uchar>(cv::Point(x, y)) = p;
			}
		}
		img = m;
	}
	else if (img.channels() == 3)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec3f pixel = img.at<cv::Vec3b>(cv::Point(x, y));

				pixel.val[0] = pixel.val[0] / 255.f;
				pixel.val[1] = pixel.val[1] / 255.f;
				pixel.val[2] = pixel.val[2] / 255.f;

				pixel.val[0] = (((pixel.val[0] > 1.0f ? exposureFactor : pixel.val[0] < 0.0f ? 0.0f : exposureFactor * pixel.val[0]) - 0.5f) * 1.0) + 0.5f;
				pixel.val[1] = (((pixel.val[1] > 1.0f ? exposureFactor : pixel.val[1] < 0.0f ? 0.0f : exposureFactor * pixel.val[1]) - 0.5f) * 1.0) + 0.5f;
				pixel.val[2] = (((pixel.val[2] > 1.0f ? exposureFactor : pixel.val[2] < 0.0f ? 0.0f : exposureFactor * pixel.val[2]) - 0.5f) * 1.0) + 0.5f;

				pixel.val[0] = pixel.val[0] > 1.0f ? 255.0f : pixel.val[0] < 0.0f ? 0.0f : 255.0f * pixel.val[0];
				pixel.val[1] = pixel.val[1] > 1.0f ? 255.0f : pixel.val[1] < 0.0f ? 0.0f : 255.0f * pixel.val[1];
				pixel.val[2] = pixel.val[2] > 1.0f ? 255.0f : pixel.val[2] < 0.0f ? 0.0f : 255.0f * pixel.val[2];

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec4f pixel = img.at<cv::Vec4b>(cv::Point(x, y));

				pixel.val[0] = pixel.val[0] / 255.f;
				pixel.val[1] = pixel.val[1] / 255.f;
				pixel.val[2] = pixel.val[2] / 255.f;

				pixel.val[0] = (((pixel.val[0] > 1.0f ? exposureFactor : pixel.val[0] < 0.0f ? 0.0f : exposureFactor * pixel.val[0]) - 0.5f) * 1.0) + 0.5f;
				pixel.val[1] = (((pixel.val[1] > 1.0f ? exposureFactor : pixel.val[1] < 0.0f ? 0.0f : exposureFactor * pixel.val[1]) - 0.5f) * 1.0) + 0.5f;
				pixel.val[2] = (((pixel.val[2] > 1.0f ? exposureFactor : pixel.val[2] < 0.0f ? 0.0f : exposureFactor * pixel.val[2]) - 0.5f) * 1.0) + 0.5f;

				pixel.val[0] = pixel.val[0] > 1.0f ? 255.0f : pixel.val[0] < 0.0f ? 0.0f : 255.0f * pixel.val[0];
				pixel.val[1] = pixel.val[1] > 1.0f ? 255.0f : pixel.val[1] < 0.0f ? 0.0f : 255.0f * pixel.val[1];
				pixel.val[2] = pixel.val[2] > 1.0f ? 255.0f : pixel.val[2] < 0.0f ? 0.0f : 255.0f * pixel.val[2];

				m.at<cv::Vec4b>(cv::Point(x, y)) = pixel;
			}
		}
		img = m;
	}
}
void fvkImageProcessing::setGammaFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	const auto v = 1.f - static_cast<double>(value) / 100.f;

	cv::Mat lut_matrix(1, 256, CV_8UC1);
	auto ptr = lut_matrix.ptr();
	for (auto i = 0; i < 256; i++)
		ptr[i] = static_cast<int>(std::pow(static_cast<double>(i) / 255.0, v) * 255.0);

	cv::Mat m(img.size(), img.type());
	cv::LUT(img, lut_matrix, m);
	img = m;
}

void fvkImageProcessing::setSaturationFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	auto v = value * -0.01f;

	if (img.channels() == 3)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec3f pixel = img.at<cv::Vec3b>(cv::Point(x, y));

				auto maxi = std::max(std::max(pixel.val[0], pixel.val[1]), pixel.val[2]);
				if (pixel.val[0] != maxi)
					pixel.val[0] += (maxi - pixel.val[0]) * v;

				if (pixel.val[1] != maxi)
					pixel.val[1] += (maxi - pixel.val[1]) * v;

				if (pixel.val[2] != maxi)
					pixel.val[2] += (maxi - pixel.val[2]) * v;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat dst;
		cv::cvtColor(img, dst, cv::COLOR_BGRA2BGR);

		cv::Mat m(dst.size(), dst.type());
		for (auto y = 0; y < dst.rows; y++)
		{
			for (auto x = 0; x < dst.cols; x++)
			{
				cv::Vec3f pixel = dst.at<cv::Vec3b>(cv::Point(x, y));

				auto maxi = std::max(std::max(pixel.val[0], pixel.val[1]), pixel.val[2]);
				if (pixel.val[0] != maxi)
					pixel.val[0] += (maxi - pixel.val[0]) * v;

				if (pixel.val[1] != maxi)
					pixel.val[1] += (maxi - pixel.val[1]) * v;

				if (pixel.val[2] != maxi)
					pixel.val[2] += (maxi - pixel.val[2]) * v;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}

		cv::cvtColor(m, m, cv::COLOR_BGR2BGRA);

		int from_to[] = { 3, 3 };
		cv::mixChannels({ {img} }, { {m} }, from_to, 1);
		img = m;
	}
}
void fvkImageProcessing::setVibranceFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	auto v = value * -1.0f;

	if (img.channels() == 3)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec3f pixel = img.at<cv::Vec3b>(cv::Point(x, y));

				auto maxi = std::max(std::max(pixel.val[0], pixel.val[1]), pixel.val[2]);

				auto avg = (pixel.val[0] + pixel.val[1] + pixel.val[2]) / 3.f;
				auto amt = ((std::abs(maxi - avg) * 2.f / 255.f) * v) / 100.f;


				if (pixel.val[0] != maxi)
					pixel.val[0] += (maxi - pixel.val[0]) * amt;

				if (pixel.val[1] != maxi)
					pixel.val[1] += (maxi - pixel.val[1]) * amt;

				if (pixel.val[2] != maxi)
					pixel.val[2] += (maxi - pixel.val[2]) * amt;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat dst;
		cv::cvtColor(img, dst, cv::COLOR_BGRA2BGR);

		cv::Mat m(dst.size(), dst.type());
		for (auto y = 0; y < dst.rows; y++)
		{
			for (auto x = 0; x < dst.cols; x++)
			{
				cv::Vec3f pixel = dst.at<cv::Vec3b>(cv::Point(x, y));

				auto maxi = std::max(std::max(pixel.val[0], pixel.val[1]), pixel.val[2]);

				auto avg = (pixel.val[0] + pixel.val[1] + pixel.val[2]) / 3.f;
				auto amt = ((std::abs(maxi - avg) * 2.f / 255.f) * v) / 100.f;


				if (pixel.val[0] != maxi)
					pixel.val[0] += (maxi - pixel.val[0]) * amt;

				if (pixel.val[1] != maxi)
					pixel.val[1] += (maxi - pixel.val[1]) * amt;

				if (pixel.val[2] != maxi)
					pixel.val[2] += (maxi - pixel.val[2]) * amt;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}

		cv::cvtColor(m, m, cv::COLOR_BGR2BGRA);

		int from_to[] = { 3, 3 };
		cv::mixChannels({ {img} }, { {m} }, from_to, 1);
		img = m;
	}
}
void fvkImageProcessing::setHueFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	if (img.channels() == 3)
	{
		cv::Mat m;
		cv::cvtColor(img, m, cv::ColorConversionCodes::COLOR_BGR2HSV);	// BGR to HSV

		for (auto y = 0; y < m.rows; y++)
		{
			for (auto x = 0; x < m.cols; x++)
			{
				cv::Vec3f pixel = m.at<cv::Vec3b>(cv::Point(x, y));

				pixel.val[0] += value;
				if (pixel.val[0] < 0) 
					pixel.val[0] = 0; 
				else if (pixel.val[0] > 255) 
					pixel.val[0] = 255;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		cv::cvtColor(m, m, cv::ColorConversionCodes::COLOR_HSV2BGR);	// HSV back to BGR
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat dst;
		cv::cvtColor(img, dst, cv::COLOR_BGRA2BGR);

		cv::Mat m;
		cv::cvtColor(dst, m, cv::ColorConversionCodes::COLOR_BGR2HSV);	// BGR to HSV

		for (auto y = 0; y < m.rows; y++)
		{
			for (auto x = 0; x < m.cols; x++)
			{
				cv::Vec3f pixel = m.at<cv::Vec3b>(cv::Point(x, y));

				pixel.val[0] += value;
				if (pixel.val[0] < 0)
					pixel.val[0] = 0;
				else if (pixel.val[0] > 255)
					pixel.val[0] = 255;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		cv::cvtColor(m, m, cv::ColorConversionCodes::COLOR_HSV2BGR);	// HSV back to BGR

		cv::cvtColor(m, m, cv::COLOR_BGR2BGRA);

		int from_to[] = { 3, 3 };
		cv::mixChannels({ {img} }, { {m} }, from_to, 1);
		img = m;
	}
}
void fvkImageProcessing::setSepiaFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0)
		return;

	auto v = static_cast<double>(value) / 100.0;

	if (img.channels() == 3)
	{
		cv::Mat m(img.size(), img.type());

		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec3d p = img.at<cv::Vec3b>(cv::Point(x, y));

				p.val[2] = std::min(255.0, (p.val[2] * (1.0 - (0.607 * v))) + (p.val[1] * (0.769 * v)) + (p.val[0] * (0.189 * v)));
				p.val[1] = std::min(255.0, (p.val[2] * (0.349 * v)) + (p.val[1] * (1.0 - (0.314 * v))) + (p.val[0] * (0.168 * v)));
				p.val[0] = std::min(255.0, (p.val[2] * (0.272 * v)) + (p.val[1] * (0.534 * v)) + (p.val[0] * (1.0 - (0.869 * v))));

				m.at<cv::Vec3b>(cv::Point(x, y)) = p;
			}
		}
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat dst;
		cv::cvtColor(img, dst, cv::COLOR_BGRA2BGR);

		cv::Mat m(dst.size(), dst.type());
		for (auto y = 0; y < dst.rows; y++)
		{
			for (auto x = 0; x < dst.cols; x++)
			{
				cv::Vec3d p = dst.at<cv::Vec3b>(cv::Point(x, y));

				p.val[2] = std::min(255.0, (p.val[2] * (1.0 - (0.607 * v))) + (p.val[1] * (0.769 * v)) + (p.val[0] * (0.189 * v)));
				p.val[1] = std::min(255.0, (p.val[2] * (0.349 * v)) + (p.val[1] * (1.0 - (0.314 * v))) + (p.val[0] * (0.168 * v)));
				p.val[0] = std::min(255.0, (p.val[2] * (0.272 * v)) + (p.val[1] * (0.534 * v)) + (p.val[0] * (1.0 - (0.869 * v))));

				m.at<cv::Vec3b>(cv::Point(x, y)) = p;
			}
		}

		cv::cvtColor(m, m, cv::COLOR_BGR2BGRA);

		int from_to[] = { 3, 3 };
		cv::mixChannels({ {img} }, { {m} }, from_to, 1);
		img = m;
	}
}

void fvkImageProcessing::setClipFilter(cv::Mat& img, int value)
{
	if (img.empty() || value == 0) 
		return;

	auto v = std::abs(static_cast<float>(value)) * 2.55f;

	if (img.channels() == 1)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				auto pixel = static_cast<float>(img.at<uchar>(cv::Point(x, y)));

				if (pixel > (255.f - v))
					pixel = 255.f;
				else if (pixel < v)
					pixel = 0.f;

				m.at<uchar>(cv::Point(x, y)) = static_cast<uchar>(pixel + 0.5f);
			}
		}
		img = m;
	}
	else if (img.channels() == 3)
	{
		cv::Mat m(img.size(), img.type());
		for (auto y = 0; y < img.rows; y++)
		{
			for (auto x = 0; x < img.cols; x++)
			{
				cv::Vec3f pixel = img.at<cv::Vec3b>(cv::Point(x, y));

				if (pixel.val[0] > (255.f - v))
					pixel.val[0] = 255.f;
				else if (pixel.val[0] < v)
					pixel.val[0] = 0.f;

				if (pixel.val[1] > (255.f - v))
					pixel.val[1] = 255.f;
				else if (pixel.val[1] < v)
					pixel.val[1] = 0.f;

				if (pixel.val[2] > (255.f - v))
					pixel.val[2] = 255.f;
				else if (pixel.val[2] < v)
					pixel.val[2] = 0.f;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat dst;
		cv::cvtColor(img, dst, cv::COLOR_BGRA2BGR);

		cv::Mat m(dst.size(), dst.type());
		for (auto y = 0; y < dst.rows; y++)
		{
			for (auto x = 0; x < dst.cols; x++)
			{
				cv::Vec3f pixel = dst.at<cv::Vec3b>(cv::Point(x, y));

				if (pixel.val[0] > (255.f - v))
					pixel.val[0] = 255.f;
				else if (pixel.val[0] < v)
					pixel.val[0] = 0.f;

				if (pixel.val[1] > (255.f - v))
					pixel.val[1] = 255.f;
				else if (pixel.val[1] < v)
					pixel.val[1] = 0.f;

				if (pixel.val[2] > (255.f - v))
					pixel.val[2] = 255.f;
				else if (pixel.val[2] < v)
					pixel.val[2] = 0.f;

				m.at<cv::Vec3b>(cv::Point(x, y)) = pixel;
			}
		}

		cv::cvtColor(m, m, cv::COLOR_BGR2BGRA);

		int from_to[] = { 3, 3 };
		cv::mixChannels({ {img} }, { {m} }, from_to, 1);
		img = m;
	}
}
void fvkImageProcessing::setEqualizeFilter(cv::Mat& img, double cliplimit, cv::Size tile_grid_size)
{
	if (img.empty() || cliplimit == 0) 
		return;

	if (img.channels() == 1)
	{
		cv::Mat m;
		auto p = cv::createCLAHE(cliplimit, tile_grid_size); // adaptive histogram equalization
		p->apply(img, m);
		img = m;
	}
	else if (img.channels() == 3)
	{
		cv::Mat m;
		std::vector<cv::Mat> channels(3);
		cv::cvtColor(img, m, cv::ColorConversionCodes::COLOR_BGR2YCrCb);	//change to YCrCb format
		cv::split(m, channels);					// split the image into channels
		auto p = cv::createCLAHE(cliplimit, tile_grid_size);
		p->apply(channels[0], channels[0]);
		cv::merge(channels, m);					// merge 3 channels including the modified one
		cv::cvtColor(m, m, cv::ColorConversionCodes::COLOR_YCrCb2BGR);		// change to BGR format
		img = m;
	}
	else if (img.channels() == 4)
	{
		cv::Mat m;
		std::vector<cv::Mat> channels(4);
		cv::split(img, channels);

		std::vector<cv::Mat> channels3(3);
		channels3[0] = channels[0];
		channels3[1] = channels[1];
		channels3[2] = channels[2];
		cv::merge(channels3, m);

		cv::cvtColor(m, m, cv::ColorConversionCodes::COLOR_BGR2YCrCb);
		cv::split(m, channels3);
		auto p = cv::createCLAHE(cliplimit, tile_grid_size);
		p->apply(channels3[0], channels3[0]);
		cv::merge(channels3, m);
		cv::cvtColor(m, m, cv::ColorConversionCodes::COLOR_YCrCb2BGR);
		
		cv::Mat dst(img.size(), img.type());
		for (auto y = 0; y < m.rows; y++)
		{
			for (auto x = 0; x < m.cols; x++)
			{
				auto pixel = m.at<cv::Vec3b>(cv::Point(x, y));
				auto p = cv::Vec4b(pixel.val[0], pixel.val[1], pixel.val[2], channels[3].at<uchar>(cv::Point(x, y)));
				dst.at<cv::Vec4b>(cv::Point(x, y)) = p;

			}
		}

		img = dst;
	}
}

void fvkImageProcessing::imageProcessing(cv::Mat& frame)
{
	m_mutex.lock();

	if (m_zoomperc > 0 && m_zoomperc != 100)
	{
		auto s = _resizeKeepAspectRatio(frame.cols, frame.rows, static_cast<int>(static_cast<float>(frame.cols * (m_zoomperc / 100.f))), static_cast<int>(static_cast<float>(frame.rows * (m_zoomperc / 100.f))));
		auto m = cv::Mat(cv::Mat::zeros(s, frame.type()));
		cv::resize(frame, m, s, 0, 0, cv::InterpolationFlags::INTER_CUBIC);
		frame = m;
	}

	if (m_flip != FlipDirection::None)
	{
		cv::Mat m;
		if (m_flip == FlipDirection::Horizontal)
			cv::flip(frame, m, 0);
		else if (m_flip == FlipDirection::Vertical)
			cv::flip(frame, m, 1);
		else if (m_flip == FlipDirection::Both)
			cv::flip(frame, m, -1);
		frame = m;
	}

	if (m_rotangle != 0)
	{
		cv::Mat m;
		if (m_rotangle == 90.)
		{
			cv::transpose(frame, m);
			cv::flip(m, m, 0);
		}
		else if (m_rotangle == 180.)
		{
			cv::flip(frame, m, -1);
		}
		else if (m_rotangle == 270.)
		{
			cv::transpose(frame, m);
			cv::flip(m, m, 1);
		}
		else
		{
			const auto cen = cv::Point2d(static_cast<double>(frame.cols) / 2.0, static_cast<double>(frame.rows) / 2.0);
			auto rot_mat = cv::getRotationMatrix2D(cen, m_rotangle, 1.0);
			const auto bbox = cv::RotatedRect(cen, frame.size(), static_cast<float>(m_rotangle)).boundingRect();
			rot_mat.at<double>(0, 2) += bbox.width / 2.0 - cen.x;
			rot_mat.at<double>(1, 2) += bbox.height / 2.0 - cen.y;
			cv::warpAffine(frame, m, rot_mat, frame.size(), cv::InterpolationFlags::INTER_LINEAR);
		}
		frame = m;
	}

	if (m_isfacetrack)
		m_ft.detect(frame, 5);

	if (m_denoislevel > 2)
		setDenoisingFilter(frame, m_denoislevel, m_denoismethod);

	if (m_smoothness > 0)
	{
		if (frame.channels() == 1 || frame.channels() == 4)
			setDenoisingFilter(frame, m_smoothness, DenoisingMethod::Gaussian);
		else
			setNonPhotorealisticFilter(frame, m_smoothness, 0.1f, fvkImageProcessing::Filters::Smoothing);	// only for 3-channels
	}

	if (m_equalizelimit > 0)
		setEqualizeFilter(frame, m_equalizelimit, cv::Size(8, 8));

	if (m_sharplevel > 0)
		setWeightedFilter(frame, m_sharplevel, 1.5, -0.5);

	if (m_details > 0)
		setNonPhotorealisticFilter(frame, m_details, 0.02f, fvkImageProcessing::Filters::Details);

	if (m_pencilsketch > 0)
		setNonPhotorealisticFilter(frame, m_pencilsketch, 0.1f, fvkImageProcessing::Filters::PencilSketch);

	if (m_stylization > 0)
		setNonPhotorealisticFilter(frame, m_stylization, 0.45f, fvkImageProcessing::Filters::Stylization);

	if (m_brigtness != 0)
		setBrightnessFilter(frame, m_brigtness);

	if (m_contrast != 0)
		setContrastFilter(frame, m_contrast);

	if (m_colorcontrast != 0)
		setColorContrastFilter(frame, m_colorcontrast);

	if (m_saturation != 0)
		setSaturationFilter(frame, m_saturation);

	if (m_vibrance != 0)
		setVibranceFilter(frame, m_vibrance);

	if (m_hue != 0)
		setHueFilter(frame, m_hue);

	if (m_exposure != 0)
		setExposureFilter(frame, m_exposure);

	if (m_gamma != 0)
		setGammaFilter(frame, m_gamma);

	if (m_sepia > 0)
		setSepiaFilter(frame, m_sepia);

	if (m_clip > 0)
		setClipFilter(frame, m_clip);

	if (m_isnegative)
	{
		cv::Mat m;
		cv::bitwise_not(frame, m);
		frame = m;
	}

	if (m_isemboss)
	{
		cv::Mat kern = (cv::Mat_<char>(3, 3) <<
			-1, -1, 0,
			-1, 0, 1,
			0, 1, 1);
		cv::Mat m;
		cv::filter2D(frame, m, frame.depth(), kern, cv::Point(-1, -1), 128);
		frame = m;
	}

	if (m_ndots > 5)
	{
		if (frame.channels() == 4)
			cv::cvtColor(frame, frame, cv::ColorConversionCodes::COLOR_BGRA2BGR);
		else if (frame.channels() == 1)
			cv::cvtColor(frame, frame, cv::ColorConversionCodes::COLOR_GRAY2BGR);

		auto dst = cv::Mat(cv::Mat::zeros(frame.size(), CV_8UC3));
		auto cir = cv::Mat(cv::Mat::zeros(frame.size(), CV_8UC1));
		auto bsize = m_ndots;

		for (auto i = 0; i < frame.rows; i += bsize)
		{
			for (auto j = 0; j < frame.cols; j += bsize)
			{
				auto rect = cv::Rect(j, i, bsize, bsize) & cv::Rect(0, 0, frame.cols, frame.rows);
				auto sub_dst = cv::Mat(dst, rect);
				sub_dst.setTo(cv::mean(frame(rect)));
				cv::circle(cir, cv::Point(j + bsize / 2, i + bsize / 2), bsize / 2 - 1, CV_RGB(255, 255, 255), -1, cv::LINE_AA);
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

		frame = dst;
	}

	if (m_convertcolor >= 0)
	{
		cv::Mat m;
		cv::cvtColor(frame, m, m_convertcolor);
		frame = m;
	}

	if (m_isgray)
	{
		cv::Mat m;
		if (frame.channels() == 3)
		{
			cv::cvtColor(frame, m, cv::ColorConversionCodes::COLOR_BGR2GRAY);
			frame = m;
		}
		else if (frame.channels() == 4)
		{
			cv::cvtColor(frame, m, cv::ColorConversionCodes::COLOR_BGRA2GRAY);
			frame = m;
		}
	}

	if (m_threshold > 0)
	{
		cv::Mat m;
		if (frame.channels() == 3)
			cv::cvtColor(frame, m, cv::ColorConversionCodes::COLOR_BGR2GRAY);
		else if (frame.channels() == 4)
			cv::cvtColor(frame, m, cv::ColorConversionCodes::COLOR_BGRA2GRAY);
		else
			m = frame;
		cv::GaussianBlur(m, m, cv::Size(5, 5), 0, 0);
		cv::threshold(m, m, 255 - m_threshold, 255, cv::THRESH_BINARY);
		frame = m;
	}

	if(m_isfacetrack)
		cv::rectangle(frame, m_ft.get().getRect(), cv::Vec3b(166, 154, 75));

	m_mutex.unlock();
}

void fvkImageProcessing::setDenoisingMethod(fvkImageProcessing::DenoisingMethod value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_denoismethod = value;
}
auto fvkImageProcessing::getDenoisingMethod() -> fvkImageProcessing::DenoisingMethod
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_denoismethod;
}
void fvkImageProcessing::setDenoisingLevel(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_denoislevel = value;
}
auto fvkImageProcessing::getDenoisingLevel() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_denoislevel;
}

void fvkImageProcessing::setSharpeningLevel(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_sharplevel = value;
}
auto fvkImageProcessing::getSharpeningLevel() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_sharplevel;
}

void fvkImageProcessing::setDetailLevel(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_details = value;
}
auto fvkImageProcessing::getDetailLevel() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_details;
}
void fvkImageProcessing::setSmoothness(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_smoothness = value;
}
auto fvkImageProcessing::getSmoothness() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_smoothness;
}
void fvkImageProcessing::setPencilSketchLevel(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_pencilsketch = value;
}
auto fvkImageProcessing::getPencilSketchLevel() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_pencilsketch;
}
void fvkImageProcessing::setStylizationLevel(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_stylization = value;
}
auto fvkImageProcessing::getStylizationLevel() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_stylization;
}

void fvkImageProcessing::setBrightness(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_brigtness = value;
}
auto fvkImageProcessing::getBrightness() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_brigtness;
}

void fvkImageProcessing::setContrast(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_contrast = value;
}
auto fvkImageProcessing::getContrast() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_contrast;
}

void fvkImageProcessing::setColorContrast(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_colorcontrast = value;
}
auto fvkImageProcessing::getColorContrast() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_colorcontrast;
}

void fvkImageProcessing::setSaturation(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_saturation = value;
}
auto fvkImageProcessing::getSaturation() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_saturation;
}

void fvkImageProcessing::setVibrance(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_vibrance = value;
}
auto fvkImageProcessing::getVibrance() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_vibrance;
}

void fvkImageProcessing::setHue(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_hue = value;
}
auto fvkImageProcessing::getHue() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_hue;
}

void fvkImageProcessing::setGamma(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_gamma = value;
}
auto fvkImageProcessing::getGamma() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_gamma;
}
void fvkImageProcessing::setExposure(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_exposure = value;
}
auto fvkImageProcessing::getExposure() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_exposure;
}

void fvkImageProcessing::setSepia(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_sepia = value;
}
auto fvkImageProcessing::getSepia() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_sepia;
}

void fvkImageProcessing::setClip(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_clip = value;
}
auto fvkImageProcessing::getClip() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_clip;
}

void fvkImageProcessing::setNegativeModeEnabled(bool value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isnegative = value;
}
auto fvkImageProcessing::isNegativeModeEnabled() -> bool
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isnegative;
}

void fvkImageProcessing::setLightEmbossEnabled(bool value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isemboss = value;
}
auto fvkImageProcessing::isLightEmbossEnabled() -> bool
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isemboss;
}

void fvkImageProcessing::setDotPatternLevel(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_ndots = value;
}
auto fvkImageProcessing::getDotPatternLevel() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_ndots;
}

void fvkImageProcessing::setFlipDirection(FlipDirection d)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_flip = d;
}
auto fvkImageProcessing::getFlipDirection() -> fvkImageProcessing::FlipDirection
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_flip;
}

void fvkImageProcessing::setZoomLevel(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_zoomperc = value;
}
auto fvkImageProcessing::getZoomLevel() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_zoomperc;
}

void fvkImageProcessing::setRotationAngle(double value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_rotangle = value;
}
auto fvkImageProcessing::getRotationAngle() -> double
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_rotangle;
}

void fvkImageProcessing::setConvertColor(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_convertcolor = value;
}
auto fvkImageProcessing::getConvertColor() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_convertcolor;
}

void fvkImageProcessing::setGrayScaleEnabled(bool value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isgray = value;
}
auto fvkImageProcessing::isGrayScaleEnabled() -> bool
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isgray;
}

void fvkImageProcessing::setThresholdValue(int value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_threshold = value;
}
auto fvkImageProcessing::getThresholdValue() -> int
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_threshold;
}
void fvkImageProcessing::setEqualizeClipLimit(double value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_equalizelimit = value;
}
auto fvkImageProcessing::getEqualizeClipLimit() -> double
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_equalizelimit;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
auto fvkImageProcessing::loadCascadeClassifier(const std::string& filename) -> bool
{
	return m_ft.loadCascadeClassifier(filename);
}
void fvkImageProcessing::setFaceDetectionEnabled(bool value)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_isfacetrack = value;
}
auto fvkImageProcessing::isFaceDetectionEnabled() -> bool
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_isfacetrack;
}
