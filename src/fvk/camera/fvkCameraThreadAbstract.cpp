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
m_isstop(false),
m_ispause(false),
m_isrepeat(true),
m_frame(cv::Mat()),
m_frame_msec(33)
{
}
fvkCameraThreadAbstract::~fvkCameraThreadAbstract()
{
	close();
}

bool fvkCameraThreadAbstract::open(int _device_id)
{
	bool r = p_cam.open(_device_id);
	if (r)
	{
		if (m_width != -1) p_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
		if (m_height != -1) p_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
		m_device_index = _device_id;
	}
	return r;
}
bool fvkCameraThreadAbstract::open()
{
	if (open(m_filepath))
		return true;
	
	return open(m_device_index);
}
bool fvkCameraThreadAbstract::open(const std::string& _avi_filename, int _api)
{
	if (_avi_filename.empty()) 
		return false;

	bool r = p_cam.open(_avi_filename, _api);
	if (r)
	{
		if (m_width != -1) p_cam.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
		if (m_height != -1) p_cam.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
		m_frame_msec = static_cast<int>(1000.0 / getFps());		// frame delay of the opened video.
		m_filepath = _avi_filename;
		m_videocapture_api = _api;
	}
	else
	{
		m_filepath = "";
	}
	return r;
}

bool fvkCameraThreadAbstract::isOpened()
{
	return p_cam.isOpened();
}

bool fvkCameraThreadAbstract::close()
{
	if (p_cam.isOpened())
	{
		p_cam.release();
		return true;
	}
	return false;
}

/************************************************************************/
/* Set Camera Settings                                                  */
/************************************************************************/
void fvkCameraThreadAbstract::openDriverConfigDialog()
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_SETTINGS, 1);
}
void fvkCameraThreadAbstract::setMsec(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_POS_MSEC, val);
}
void fvkCameraThreadAbstract::setPosFrames(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_POS_FRAMES, val);
}

void fvkCameraThreadAbstract::setSharpness(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_SHARPNESS, val);
}
void fvkCameraThreadAbstract::setAutoExposure(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_AUTO_EXPOSURE, val);
}
void fvkCameraThreadAbstract::setFps(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_FPS, val);
}
void fvkCameraThreadAbstract::setFrameCount(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_FRAME_COUNT, val);
}

void fvkCameraThreadAbstract::setWhiteBalanceBlueU(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_WHITE_BALANCE_BLUE_U, val);
}
void fvkCameraThreadAbstract::setWhiteBalanceRedV(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_WHITE_BALANCE_RED_V, val);
}
void fvkCameraThreadAbstract::setFourCC(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_FOURCC, val);
}
void fvkCameraThreadAbstract::setConvertToRGB(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_CONVERT_RGB, val);
}
void fvkCameraThreadAbstract::setBrightness(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_BRIGHTNESS, val);
}
void fvkCameraThreadAbstract::setContrast(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_CONTRAST, val);
}
void fvkCameraThreadAbstract::setSaturation(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_SATURATION, val);
}
void fvkCameraThreadAbstract::setHue(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_HUE, val);
}
void fvkCameraThreadAbstract::setGain(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_GAIN, val);
}
void fvkCameraThreadAbstract::setExposure(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_EXPOSURE, val);
}
void fvkCameraThreadAbstract::setRectification(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_RECTIFICATION, val);
}
void fvkCameraThreadAbstract::setFormat(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_FORMAT, val);
}
void fvkCameraThreadAbstract::setMode(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_MODE, val);
}

void fvkCameraThreadAbstract::setGamma(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_GAMMA, val);
}
void fvkCameraThreadAbstract::setTemperature(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_TEMPERATURE, val);
}
void fvkCameraThreadAbstract::setZoom(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_ZOOM, val);
}
void fvkCameraThreadAbstract::setFocus(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_FOCUS, val);
}
void fvkCameraThreadAbstract::setIsoSpeed(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_ISO_SPEED, val);
}
void fvkCameraThreadAbstract::setBackLight(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_BACKLIGHT, val);
}
void fvkCameraThreadAbstract::setPan(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_PAN, val);
}
void fvkCameraThreadAbstract::setTilt(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_TILT, val);
}
void fvkCameraThreadAbstract::setRoll(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_ROLL, val);
}
void fvkCameraThreadAbstract::setTrigger(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_TRIGGER, val);
}
void fvkCameraThreadAbstract::setTriggerDelay(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_TRIGGER_DELAY, val);
}
void fvkCameraThreadAbstract::setAviRatio(double val)
{
	if (isOpened()) p_cam.set(cv::CAP_PROP_POS_AVI_RATIO, val);
}


/************************************************************************/
/* Get Camera Settings                                                  */
/************************************************************************/
cv::Size fvkCameraThreadAbstract::getResolution()
{
	cv::Size size(m_width, m_height);
	if(!isOpened()) return size;
	return cv::Size(static_cast<int>(p_cam.get(cv::CAP_PROP_FRAME_WIDTH)), static_cast<int>(p_cam.get(cv::CAP_PROP_FRAME_HEIGHT)));
}
int fvkCameraThreadAbstract::getWidth()
{
	return static_cast<int>(p_cam.get(cv::CAP_PROP_FRAME_WIDTH));
}
int fvkCameraThreadAbstract::getHeight()
{
	return static_cast<int>(p_cam.get(cv::CAP_PROP_FRAME_HEIGHT));
}
double fvkCameraThreadAbstract::getSharpness()
{
    return p_cam.get(cv::CAP_PROP_SHARPNESS);
}
double fvkCameraThreadAbstract::getAutoExposure()
{
    return p_cam.get(cv::CAP_PROP_AUTO_EXPOSURE);
}
double fvkCameraThreadAbstract::getFps()
{
    return p_cam.get(cv::CAP_PROP_FPS);
}
double fvkCameraThreadAbstract::getFrameCount()
{
	return p_cam.get(cv::CAP_PROP_FRAME_COUNT);
}
double fvkCameraThreadAbstract::getWhiteBalanceBlueU()
{
    return p_cam.get(cv::CAP_PROP_WHITE_BALANCE_BLUE_U);
}
double fvkCameraThreadAbstract::getWhiteBalanceRedV()
{
    return p_cam.get(cv::CAP_PROP_WHITE_BALANCE_RED_V);
}
std::string fvkCameraThreadAbstract::getFourCC()
{
	unsigned int f = static_cast<unsigned int>(p_cam.get(cv::CAP_PROP_FOURCC));
	char fourc[] =
	{
		(char)f,
		(char)(f >> 8),
		(char)(f >> 16),
		(char)(f >> 24),
		'\0',
	};
	return std::string(fourc);
}

double fvkCameraThreadAbstract::getConvertToRGB()
{
	return p_cam.get(cv::CAP_PROP_CONVERT_RGB);
}
double fvkCameraThreadAbstract::getBrightness()
{
	return p_cam.get(cv::CAP_PROP_BRIGHTNESS);
}
double fvkCameraThreadAbstract::getContrast()
{
	return p_cam.get(cv::CAP_PROP_CONTRAST);
}
double fvkCameraThreadAbstract::getSaturation()
{
	return p_cam.get(cv::CAP_PROP_SATURATION);
}
double fvkCameraThreadAbstract::getHue()
{
	return p_cam.get(cv::CAP_PROP_HUE);
}
double fvkCameraThreadAbstract::getGain()
{
	return p_cam.get(cv::CAP_PROP_GAIN);
}
double fvkCameraThreadAbstract::getExposure()
{
	return p_cam.get(cv::CAP_PROP_EXPOSURE);
}
double fvkCameraThreadAbstract::getRectification()
{
	return p_cam.get(cv::CAP_PROP_RECTIFICATION);
}
double fvkCameraThreadAbstract::getFormat()
{
	return p_cam.get(cv::CAP_PROP_FORMAT);
}
double fvkCameraThreadAbstract::getMode()
{
	return p_cam.get(cv::CAP_PROP_MODE);
}
double fvkCameraThreadAbstract::getGamma()
{
	return p_cam.get(cv::CAP_PROP_GAMMA);
}
double fvkCameraThreadAbstract::getTemperature()
{
	return p_cam.get(cv::CAP_PROP_TEMPERATURE);
}
double fvkCameraThreadAbstract::getZoom()
{
	return p_cam.get(cv::CAP_PROP_ZOOM);
}
double fvkCameraThreadAbstract::getFocus()
{
	return p_cam.get(cv::CAP_PROP_FOCUS);
}
double fvkCameraThreadAbstract::getIsoSpeed()
{
	return p_cam.get(cv::CAP_PROP_ISO_SPEED);
}
double fvkCameraThreadAbstract::getBackLight()
{
	return p_cam.get(cv::CAP_PROP_BACKLIGHT);
}
double fvkCameraThreadAbstract::getPan()
{
	return p_cam.get(cv::CAP_PROP_PAN);
}
double fvkCameraThreadAbstract::getTilt()
{
	return p_cam.get(cv::CAP_PROP_TILT);
}
double fvkCameraThreadAbstract::getRoll()
{
	return p_cam.get(cv::CAP_PROP_ROLL);
}
double fvkCameraThreadAbstract::getTrigger()
{
	return p_cam.get(cv::CAP_PROP_TRIGGER);
}
double fvkCameraThreadAbstract::getTriggerDelay()
{
	return p_cam.get(cv::CAP_PROP_TRIGGER_DELAY);
}
double fvkCameraThreadAbstract::getMsec()
{
	return p_cam.get(CV_CAP_PROP_POS_MSEC);
}
double fvkCameraThreadAbstract::getPosFrames()
{
	return p_cam.get(CV_CAP_PROP_POS_FRAMES);
}
double fvkCameraThreadAbstract::getAviRatio()
{
	return p_cam.get(CV_CAP_PROP_POS_AVI_RATIO);
}