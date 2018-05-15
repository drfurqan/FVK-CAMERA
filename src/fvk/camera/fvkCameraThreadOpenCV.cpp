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

fvkCameraThreadOpenCV::fvkCameraThreadOpenCV(const int _device_index, const cv::Size& _resolution, fvkSemaphoreBuffer<cv::Mat>* _buffer) : 
	fvkCameraThread(_device_index, _resolution, _buffer),
	m_videocapture_api(static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY)),
	m_filepath(""),
	m_isrepeat(true)
{
}

fvkCameraThreadOpenCV::fvkCameraThreadOpenCV(const std::string& _video_file, const cv::Size& _resolution, const int _api, fvkSemaphoreBuffer<cv::Mat>* _buffer) :
	fvkCameraThread(0, _resolution, _buffer),
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
	const auto b = m_cam.open(_device_index);
	if (!b) return false;

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
auto fvkCameraThreadOpenCV::open(const std::string& _file_name, const int _api) -> bool
{
	if (_file_name.empty())
		return false;

	if (m_cam.open(_file_name, _api))
	{
		if (m_frame_size.width != -1)
			m_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_frame_size.width);
		if (m_frame_size.height != -1)
			m_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_frame_size.height);

		// not every frame resolution is supported by the video devices, so just update the
		// frame size with the new supported frame resolution.
		m_frame_size.width = static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_WIDTH));
		m_frame_size.height = static_cast<int>(m_cam.get(cv::CAP_PROP_FRAME_HEIGHT));

		m_filepath = _file_name;
		m_videocapture_api = _api;
		setDelay(static_cast<int>(1000.0 / m_cam.get(cv::CAP_PROP_FPS)));	// delay between frames.

		return true;
	}

	return false;
}
auto fvkCameraThreadOpenCV::open() -> bool
{
	if (open(m_filepath, m_videocapture_api))
		return true;

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
void fvkCameraThreadOpenCV::openDriverConfigDialog()
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_SETTINGS, 1);
}
void fvkCameraThreadOpenCV::setMsec(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_POS_MSEC, val);
}
void fvkCameraThreadOpenCV::setPosFrames(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_POS_FRAMES, val);
}

void fvkCameraThreadOpenCV::setSharpness(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_SHARPNESS, val);
}
void fvkCameraThreadOpenCV::setAutoExposure(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_AUTO_EXPOSURE, val);
}
void fvkCameraThreadOpenCV::setFps(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FPS, val);
}
void fvkCameraThreadOpenCV::setFrameCount(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FRAME_COUNT, val);
}

void fvkCameraThreadOpenCV::setWhiteBalanceBlueU(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_WHITE_BALANCE_BLUE_U, val);
}
void fvkCameraThreadOpenCV::setWhiteBalanceRedV(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_WHITE_BALANCE_RED_V, val);
}
void fvkCameraThreadOpenCV::setFourCC(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FOURCC, val);
}
void fvkCameraThreadOpenCV::setConvertToRGB(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_CONVERT_RGB, val);
}
void fvkCameraThreadOpenCV::setBrightness(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_BRIGHTNESS, val);
}
void fvkCameraThreadOpenCV::setContrast(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_CONTRAST, val);
}
void fvkCameraThreadOpenCV::setSaturation(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_SATURATION, val);
}
void fvkCameraThreadOpenCV::setHue(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_HUE, val);
}
void fvkCameraThreadOpenCV::setGain(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_GAIN, val);
}
void fvkCameraThreadOpenCV::setExposure(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_EXPOSURE, val);
}
void fvkCameraThreadOpenCV::setRectification(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_RECTIFICATION, val);
}
void fvkCameraThreadOpenCV::setFormat(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FORMAT, val);
}
void fvkCameraThreadOpenCV::setMode(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_MODE, val);
}

void fvkCameraThreadOpenCV::setGamma(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_GAMMA, val);
}
void fvkCameraThreadOpenCV::setTemperature(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TEMPERATURE, val);
}
void fvkCameraThreadOpenCV::setZoom(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_ZOOM, val);
}
void fvkCameraThreadOpenCV::setFocus(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_FOCUS, val);
}
void fvkCameraThreadOpenCV::setIsoSpeed(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_ISO_SPEED, val);
}
void fvkCameraThreadOpenCV::setBackLight(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_BACKLIGHT, val);
}
void fvkCameraThreadOpenCV::setPan(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_PAN, val);
}
void fvkCameraThreadOpenCV::setTilt(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TILT, val);
}
void fvkCameraThreadOpenCV::setRoll(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_ROLL, val);
}
void fvkCameraThreadOpenCV::setTrigger(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TRIGGER, val);
}
void fvkCameraThreadOpenCV::setTriggerDelay(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_TRIGGER_DELAY, val);
}
void fvkCameraThreadOpenCV::setAviRatio(double val)
{
	if (isOpened()) m_cam.set(cv::CAP_PROP_POS_AVI_RATIO, val);
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
	return m_cam.get(CV_CAP_PROP_POS_MSEC);
}
auto fvkCameraThreadOpenCV::getPosFrames() const -> double
{
	return m_cam.get(CV_CAP_PROP_POS_FRAMES);
}
auto fvkCameraThreadOpenCV::getAviRatio() const -> double
{
	return m_cam.get(CV_CAP_PROP_POS_AVI_RATIO);
}