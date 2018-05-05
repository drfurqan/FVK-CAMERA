/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2017/02/09   11:18PM
filename: 	fvkCameraThreadAbstract.cpp
file base:	fvkCameraThreadAbstract
file ext:	cpp
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

#include <fvk/camera/fvkCameraThreadAbstract.h>

using namespace R3D;

fvkCameraThreadAbstract::fvkCameraThreadAbstract() :
m_device_index(0),
m_width(-1),
m_height(-1),
m_filepath(""),
m_videocapture_api(cv::VideoCaptureAPIs::CAP_ANY),
m_frame(cv::Mat())
{
}
fvkCameraThreadAbstract::~fvkCameraThreadAbstract()
{
	close();
}

auto fvkCameraThreadAbstract::open(int _device_id) -> bool
{
	const auto r = m_cam.open(_device_id);
	if (r)
	{
		if (m_width != -1) m_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
		if (m_height != -1) m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
		m_device_index = _device_id;
	}
	return r;
}
auto fvkCameraThreadAbstract::open() -> bool
{
	if (open(m_filepath))
		return true;
	
	return open(m_device_index);
}
auto fvkCameraThreadAbstract::open(const std::string& _avi_filename, int _api) -> bool
{
	if (_avi_filename.empty()) 
		return false;

	const auto r = m_cam.open(_avi_filename, _api);
	if (r)
	{
		if (m_width != -1) m_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
		if (m_height != -1) m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
		m_filepath = _avi_filename;
		m_videocapture_api = _api;
	}
	else
	{
		m_filepath = "";
	}
	return r;
}

auto fvkCameraThreadAbstract::isOpened() const -> bool
{
	return m_cam.isOpened();
}

auto fvkCameraThreadAbstract::close() -> bool
{
	if (m_cam.isOpened())
	{
		m_cam.release();
		return true;
	}
	return false;
}

/************************************************************************/
/* Set Camera Settings                                                  */
/************************************************************************/
void fvkCameraThreadAbstract::openDriverConfigDialog()
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_SETTINGS, 1);
}
void fvkCameraThreadAbstract::setMsec(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_POS_MSEC, val);
}
void fvkCameraThreadAbstract::setPosFrames(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_POS_FRAMES, val);
}

void fvkCameraThreadAbstract::setSharpness(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_SHARPNESS, val);
}
void fvkCameraThreadAbstract::setAutoExposure(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_AUTO_EXPOSURE, val);
}
void fvkCameraThreadAbstract::setFps(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FPS, val);
}
void fvkCameraThreadAbstract::setFrameCount(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FRAME_COUNT, val);
}

void fvkCameraThreadAbstract::setWhiteBalanceBlueU(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_WHITE_BALANCE_BLUE_U, val);
}
void fvkCameraThreadAbstract::setWhiteBalanceRedV(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_WHITE_BALANCE_RED_V, val);
}
void fvkCameraThreadAbstract::setFourCC(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FOURCC, val);
}
void fvkCameraThreadAbstract::setConvertToRGB(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_CONVERT_RGB, val);
}
void fvkCameraThreadAbstract::setBrightness(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_BRIGHTNESS, val);
}
void fvkCameraThreadAbstract::setContrast(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_CONTRAST, val);
}
void fvkCameraThreadAbstract::setSaturation(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_SATURATION, val);
}
void fvkCameraThreadAbstract::setHue(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_HUE, val);
}
void fvkCameraThreadAbstract::setGain(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_GAIN, val);
}
void fvkCameraThreadAbstract::setExposure(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_EXPOSURE, val);
}
void fvkCameraThreadAbstract::setRectification(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_RECTIFICATION, val);
}
void fvkCameraThreadAbstract::setFormat(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FORMAT, val);
}
void fvkCameraThreadAbstract::setMode(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_MODE, val);
}

void fvkCameraThreadAbstract::setGamma(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_GAMMA, val);
}
void fvkCameraThreadAbstract::setTemperature(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TEMPERATURE, val);
}
void fvkCameraThreadAbstract::setZoom(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_ZOOM, val);
}
void fvkCameraThreadAbstract::setFocus(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FOCUS, val);
}
void fvkCameraThreadAbstract::setIsoSpeed(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_ISO_SPEED, val);
}
void fvkCameraThreadAbstract::setBackLight(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_BACKLIGHT, val);
}
void fvkCameraThreadAbstract::setPan(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_PAN, val);
}
void fvkCameraThreadAbstract::setTilt(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TILT, val);
}
void fvkCameraThreadAbstract::setRoll(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_ROLL, val);
}
void fvkCameraThreadAbstract::setTrigger(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TRIGGER, val);
}
void fvkCameraThreadAbstract::setTriggerDelay(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TRIGGER_DELAY, val);
}
void fvkCameraThreadAbstract::setAviRatio(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_POS_AVI_RATIO, val);
}


/************************************************************************/
/* Get Camera Settings                                                  */
/************************************************************************/
auto fvkCameraThreadAbstract::getResolution() const -> cv::Size
{
	cv::Size size(m_width, m_height);
	if(!isOpened()) return size;
	return cv::Size(static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_WIDTH)), static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_HEIGHT)));
}
auto fvkCameraThreadAbstract::getWidth() const -> int
{
	return static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_WIDTH));
}
auto fvkCameraThreadAbstract::getHeight() const -> int
{
	return static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_HEIGHT));
}
auto fvkCameraThreadAbstract::getSharpness() const -> double
{
	return m_cam.get(cv::CAP_PROP_SHARPNESS);
}
auto fvkCameraThreadAbstract::getAutoExposure() const -> double
{
	return m_cam.get(cv::CAP_PROP_AUTO_EXPOSURE);
}
auto fvkCameraThreadAbstract::getFps() const -> double
{
	return m_cam.get(cv::CAP_PROP_FPS);
}
auto fvkCameraThreadAbstract::getFrameCount() const -> double
{
	return m_cam.get(cv::CAP_PROP_FRAME_COUNT);
}
auto fvkCameraThreadAbstract::getWhiteBalanceBlueU() const -> double
{
	return m_cam.get(cv::CAP_PROP_WHITE_BALANCE_BLUE_U);
}
auto fvkCameraThreadAbstract::getWhiteBalanceRedV() const -> double
{
	return m_cam.get(cv::CAP_PROP_WHITE_BALANCE_RED_V);
}
auto fvkCameraThreadAbstract::getFourCC() const -> std::string
{
	auto f = static_cast<unsigned int>(m_cam.get(cv::CAP_PROP_FOURCC));
	char fourc[] =
	{
		static_cast<char>(f),
		static_cast<char>(f >> 8),
		static_cast<char>(f >> 16),
		static_cast<char>(f >> 24),
		'\0',
	};
	return std::string(fourc);
}

auto fvkCameraThreadAbstract::getConvertToRGB() const -> double
{
	return m_cam.get(cv::CAP_PROP_CONVERT_RGB);
}
auto fvkCameraThreadAbstract::getBrightness() const -> double
{
	return m_cam.get(cv::CAP_PROP_BRIGHTNESS);
}
auto fvkCameraThreadAbstract::getContrast() const -> double
{
	return m_cam.get(cv::CAP_PROP_CONTRAST);
}
auto fvkCameraThreadAbstract::getSaturation() const -> double
{
	return m_cam.get(cv::CAP_PROP_SATURATION);
}
auto fvkCameraThreadAbstract::getHue() const -> double
{
	return m_cam.get(cv::CAP_PROP_HUE);
}
auto fvkCameraThreadAbstract::getGain() const -> double
{
	return m_cam.get(cv::CAP_PROP_GAIN);
}
auto fvkCameraThreadAbstract::getExposure() const -> double
{
	return m_cam.get(cv::CAP_PROP_EXPOSURE);
}
auto fvkCameraThreadAbstract::getRectification() const -> double
{
	return m_cam.get(cv::CAP_PROP_RECTIFICATION);
}
auto fvkCameraThreadAbstract::getFormat() const -> double
{
	return m_cam.get(cv::CAP_PROP_FORMAT);
}
auto fvkCameraThreadAbstract::getMode() const -> double
{
	return m_cam.get(cv::CAP_PROP_MODE);
}
auto fvkCameraThreadAbstract::getGamma() const -> double
{
	return m_cam.get(cv::CAP_PROP_GAMMA);
}
auto fvkCameraThreadAbstract::getTemperature() const -> double
{
	return m_cam.get(cv::CAP_PROP_TEMPERATURE);
}
auto fvkCameraThreadAbstract::getZoom() const -> double
{
	return m_cam.get(cv::CAP_PROP_ZOOM);
}
auto fvkCameraThreadAbstract::getFocus() const -> double
{
	return m_cam.get(cv::CAP_PROP_FOCUS);
}
auto fvkCameraThreadAbstract::getIsoSpeed() const -> double
{
	return m_cam.get(cv::CAP_PROP_ISO_SPEED);
}
auto fvkCameraThreadAbstract::getBackLight() const -> double
{
	return m_cam.get(cv::CAP_PROP_BACKLIGHT);
}
auto fvkCameraThreadAbstract::getPan() const -> double
{
	return m_cam.get(cv::CAP_PROP_PAN);
}
auto fvkCameraThreadAbstract::getTilt() const -> double
{
	return m_cam.get(cv::CAP_PROP_TILT);
}
auto fvkCameraThreadAbstract::getRoll() const -> double
{
	return m_cam.get(cv::CAP_PROP_ROLL);
}
auto fvkCameraThreadAbstract::getTrigger() const -> double
{
	return m_cam.get(cv::CAP_PROP_TRIGGER);
}
auto fvkCameraThreadAbstract::getTriggerDelay() const -> double
{
	return m_cam.get(cv::CAP_PROP_TRIGGER_DELAY);
}
auto fvkCameraThreadAbstract::getMsec() const -> double
{
	return m_cam.get(CV_CAP_PROP_POS_MSEC);
}
auto fvkCameraThreadAbstract::getPosFrames() const -> double
{
	return m_cam.get(CV_CAP_PROP_POS_FRAMES);
}
auto fvkCameraThreadAbstract::getAviRatio() const -> double
{
	return m_cam.get(CV_CAP_PROP_POS_AVI_RATIO);
}