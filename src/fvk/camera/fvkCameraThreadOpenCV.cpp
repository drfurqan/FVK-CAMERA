/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2018/05/07   11:18PM
filename: 	fvkCameraThreadOpenCV.cpp
file base:	fvkCameraThreadOpenCV
file ext:	cpp
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

#include <fvk/camera/fvkCameraThreadOpenCV.h>

using namespace R3D;

fvkCameraThreadOpenCV::fvkCameraThreadOpenCV(const int _device_index, const cv::Size& _frame_size, const int _api, fvkSemaphoreBuffer<cv::Mat>* _buffer) :
	fvkCameraThread(_device_index, _frame_size, _buffer),
	m_videocapture_api(_api),
	m_filepath(""),
	m_isrepeat(true)
{
}

fvkCameraThreadOpenCV::fvkCameraThreadOpenCV(const std::string& _video_file, const cv::Size& _frame_size, const int _api, fvkSemaphoreBuffer<cv::Mat>* _buffer) :
	fvkCameraThread(0, _frame_size, _buffer),
	m_videocapture_api(_api),
	m_filepath(_video_file),
	m_isrepeat(true)
{
}

fvkCameraThreadOpenCV::~fvkCameraThreadOpenCV()
{
	stop();
	fvkCameraThreadOpenCV::close();
}

auto fvkCameraThreadOpenCV::open(const int _device_index) -> bool
{
	if (!m_cam.open(_device_index, m_videocapture_api)) 
		return false;

	if (m_frame_size.width != -1)
		m_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_frame_size.width);
	if (m_frame_size.height != -1)
		m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_frame_size.height);

	// not every frame resolution is supported by the camera devices, so just update the
	// frame size with the new supported frame resolution.
	m_frame_size.width = static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_WIDTH));
	m_frame_size.height = static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_HEIGHT));

	// on some camera devices, CAP_PROP_FRAME_WIDTH and CAP_PROP_FRAME_HEIGHT doesn't work,
	// so just try to get the default frame size.
	if(m_frame_size.width == 0 || m_frame_size.height == 0)
	{
		m_frame_size.width = 640;
		m_frame_size.height = 480;
		m_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_frame_size.width);
		m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_frame_size.height);
	}

	m_device_index = _device_index;

	return true;
}
auto fvkCameraThreadOpenCV::open(const std::string& _file_name) -> bool
{
	if (_file_name.empty())
		return false;

	if (!m_cam.open(_file_name, m_videocapture_api))
		return false;

	if (m_frame_size.width != -1)
		m_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_frame_size.width);
	if (m_frame_size.height != -1)
		m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_frame_size.height);

	// not every frame resolution is supported by the video devices, so just update the
	// frame size with the new supported frame resolution.
	m_frame_size.width = static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_WIDTH));
	m_frame_size.height = static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_HEIGHT));

	setDelay(static_cast<int>(1000.0 / m_cam.get(cv::CAP_PROP_FPS)));	// delay between frames.

	m_filepath = _file_name;

	return true;
}
auto fvkCameraThreadOpenCV::open() -> bool
{
	// if user specify the video file location then try to open the video file
	// otherwise try to open the camera by camera device index.
	if (!m_filepath.empty())
	{
		return open(m_filepath);
	}

	return open(m_device_index);
}
auto fvkCameraThreadOpenCV::isOpened() const -> bool
{
	return m_cam.isOpened();
}
auto fvkCameraThreadOpenCV::close() -> bool
{
	if (!isOpened())
		return false;

	m_cam.release();
	return true;
}

auto fvkCameraThreadOpenCV::grab(cv::Mat& _m_frame) -> bool
{
	// grab from the video file.
	if (!m_filepath.empty())					// if there is a *.avi video file, then grab from it.
	{
		if (!m_cam.grab())						// capture frame (if available).
		{
			if (m_isrepeat)
			{
				setPosFrames(0);				// reset the camera frame to 0.
				setFrameNumber(0);				// reset the camera frame to 0.
				return false;
			}
		}
	}
	else										// otherwise, grab from the camera device.
	{
		if (!m_cam.grab())						// capture frame (if available).
			return false;
	}

	return m_cam.retrieve(_m_frame);
}

void fvkCameraThreadOpenCV::repeat(const bool _b)
{
	m_isrepeat = _b;
}
auto fvkCameraThreadOpenCV::repeat() const -> bool
{
	return m_isrepeat;
}

/************************************************************************/
/* Set Camera Settings                                                  */
/************************************************************************/
auto fvkCameraThreadOpenCV::openConfigurationDialog() -> bool
{
	return m_cam.set(cv::CAP_PROP_SETTINGS, 1);
}
auto fvkCameraThreadOpenCV::setMsec(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_POS_MSEC, val);
}
auto fvkCameraThreadOpenCV::setPosFrames(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_POS_FRAMES, val);
}

auto fvkCameraThreadOpenCV::setSharpness(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_SHARPNESS, val);
}
auto fvkCameraThreadOpenCV::setAutoExposure(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_AUTO_EXPOSURE, val);
}
auto fvkCameraThreadOpenCV::setFps(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_FPS, val);
}
auto fvkCameraThreadOpenCV::setFrameCount(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_FRAME_COUNT, val);
}

auto fvkCameraThreadOpenCV::setWhiteBalanceBlueU(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_WHITE_BALANCE_BLUE_U, val);
}
auto fvkCameraThreadOpenCV::setWhiteBalanceRedV(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_WHITE_BALANCE_RED_V, val);
}
auto fvkCameraThreadOpenCV::setFourCC(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_FOURCC, val);
}
auto fvkCameraThreadOpenCV::setConvertToRGB(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_CONVERT_RGB, val);
}
auto fvkCameraThreadOpenCV::setBrightness(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_BRIGHTNESS, val);
}
auto fvkCameraThreadOpenCV::setContrast(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_CONTRAST, val);
}
auto fvkCameraThreadOpenCV::setSaturation(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_SATURATION, val);
}
auto fvkCameraThreadOpenCV::setHue(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_HUE, val);
}
auto fvkCameraThreadOpenCV::setGain(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_GAIN, val);
}
auto fvkCameraThreadOpenCV::setExposure(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_EXPOSURE, val);
}
auto fvkCameraThreadOpenCV::setRectification(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_RECTIFICATION, val);
}
auto fvkCameraThreadOpenCV::setFormat(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_FORMAT, val);
}
auto fvkCameraThreadOpenCV::setMode(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_MODE, val);
}

auto fvkCameraThreadOpenCV::setGamma(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_GAMMA, val);
}
auto fvkCameraThreadOpenCV::setTemperature(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_TEMPERATURE, val);
}
auto fvkCameraThreadOpenCV::setZoom(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_ZOOM, val);
}
auto fvkCameraThreadOpenCV::setFocus(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_FOCUS, val);
}
auto fvkCameraThreadOpenCV::setIsoSpeed(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_ISO_SPEED, val);
}
auto fvkCameraThreadOpenCV::setBackLight(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_BACKLIGHT, val);
}
auto fvkCameraThreadOpenCV::setPan(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_PAN, val);
}
auto fvkCameraThreadOpenCV::setTilt(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_TILT, val);
}
auto fvkCameraThreadOpenCV::setRoll(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_ROLL, val);
}
auto fvkCameraThreadOpenCV::setTrigger(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_TRIGGER, val);
}
auto fvkCameraThreadOpenCV::setTriggerDelay(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_TRIGGER_DELAY, val);
}
auto fvkCameraThreadOpenCV::setAviRatio(double val) -> bool
{
	return m_cam.set(cv::CAP_PROP_POS_AVI_RATIO, val);
}


/************************************************************************/
/* Get Camera Settings                                                  */
/************************************************************************/
auto fvkCameraThreadOpenCV::getActiveFrameSize() const -> cv::Size
{
	if(!isOpened()) return cv::Size(0, 0);

	return cv::Size(static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_WIDTH)), static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_HEIGHT)));
}
auto fvkCameraThreadOpenCV::getSharpness() const -> double
{
	return m_cam.get(cv::CAP_PROP_SHARPNESS);
}
auto fvkCameraThreadOpenCV::getAutoExposure() const -> double
{
	return m_cam.get(cv::CAP_PROP_AUTO_EXPOSURE);
}
auto fvkCameraThreadOpenCV::getFps() const -> double
{
	return m_cam.get(cv::CAP_PROP_FPS);
}
auto fvkCameraThreadOpenCV::getFrameCount() const -> double
{
	return m_cam.get(cv::CAP_PROP_FRAME_COUNT);
}
auto fvkCameraThreadOpenCV::getWhiteBalanceBlueU() const -> double
{
	return m_cam.get(cv::CAP_PROP_WHITE_BALANCE_BLUE_U);
}
auto fvkCameraThreadOpenCV::getWhiteBalanceRedV() const -> double
{
	return m_cam.get(cv::CAP_PROP_WHITE_BALANCE_RED_V);
}
auto fvkCameraThreadOpenCV::getFourCC() const -> std::string
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
auto fvkCameraThreadOpenCV::getConvertToRGB() const -> double
{
	return m_cam.get(cv::CAP_PROP_CONVERT_RGB);
}
auto fvkCameraThreadOpenCV::getBrightness() const -> double
{
	return m_cam.get(cv::CAP_PROP_BRIGHTNESS);
}
auto fvkCameraThreadOpenCV::getContrast() const -> double
{
	return m_cam.get(cv::CAP_PROP_CONTRAST);
}
auto fvkCameraThreadOpenCV::getSaturation() const -> double
{
	return m_cam.get(cv::CAP_PROP_SATURATION);
}
auto fvkCameraThreadOpenCV::getHue() const -> double
{
	return m_cam.get(cv::CAP_PROP_HUE);
}
auto fvkCameraThreadOpenCV::getGain() const -> double
{
	return m_cam.get(cv::CAP_PROP_GAIN);
}
auto fvkCameraThreadOpenCV::getExposure() const -> double
{
	return m_cam.get(cv::CAP_PROP_EXPOSURE);
}
auto fvkCameraThreadOpenCV::getRectification() const -> double
{
	return m_cam.get(cv::CAP_PROP_RECTIFICATION);
}
auto fvkCameraThreadOpenCV::getFormat() const -> double
{
	return m_cam.get(cv::CAP_PROP_FORMAT);
}
auto fvkCameraThreadOpenCV::getMode() const -> double
{
	return m_cam.get(cv::CAP_PROP_MODE);
}
auto fvkCameraThreadOpenCV::getGamma() const -> double
{
	return m_cam.get(cv::CAP_PROP_GAMMA);
}
auto fvkCameraThreadOpenCV::getTemperature() const -> double
{
	return m_cam.get(cv::CAP_PROP_TEMPERATURE);
}
auto fvkCameraThreadOpenCV::getZoom() const -> double
{
	return m_cam.get(cv::CAP_PROP_ZOOM);
}
auto fvkCameraThreadOpenCV::getFocus() const -> double
{
	return m_cam.get(cv::CAP_PROP_FOCUS);
}
auto fvkCameraThreadOpenCV::getIsoSpeed() const -> double
{
	return m_cam.get(cv::CAP_PROP_ISO_SPEED);
}
auto fvkCameraThreadOpenCV::getBackLight() const -> double
{
	return m_cam.get(cv::CAP_PROP_BACKLIGHT);
}
auto fvkCameraThreadOpenCV::getPan() const -> double
{
	return m_cam.get(cv::CAP_PROP_PAN);
}
auto fvkCameraThreadOpenCV::getTilt() const -> double
{
	return m_cam.get(cv::CAP_PROP_TILT);
}
auto fvkCameraThreadOpenCV::getRoll() const -> double
{
	return m_cam.get(cv::CAP_PROP_ROLL);
}
auto fvkCameraThreadOpenCV::getTrigger() const -> double
{
	return m_cam.get(cv::CAP_PROP_TRIGGER);
}
auto fvkCameraThreadOpenCV::getTriggerDelay() const -> double
{
	return m_cam.get(cv::CAP_PROP_TRIGGER_DELAY);
}
auto fvkCameraThreadOpenCV::getMsec() const -> double
{
	return m_cam.get(cv::CAP_PROP_POS_MSEC);
}
auto fvkCameraThreadOpenCV::getPosFrames() const -> double
{
	return m_cam.get(cv::CAP_PROP_POS_FRAMES);
}
auto fvkCameraThreadOpenCV::getAviRatio() const -> double
{
	return m_cam.get(cv::CAP_PROP_POS_AVI_RATIO);
}