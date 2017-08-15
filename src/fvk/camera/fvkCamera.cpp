/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2017/02/09   12:03AM
filename: 	fvkCamera.cpp
file base:	fvkCamera
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Thread-safe camera class using OpenCV.
			Capturing is done on one thread and captured frame processing on the other.
			Both thread are synchronized with semaphore methodology.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkCamera.h>
#include <fvk/camera/fvkCameraProcessingThread.h>
//#include <future>

using namespace R3D;

fvkCamera::fvkCamera(int _device_id, cv::Size _resolution) : 
fvkCameraAbstract(),
p_stdct(nullptr),
p_stdpt(nullptr)
{
	fvkSemaphoreBuffer<cv::Mat>* b = new fvkSemaphoreBuffer<cv::Mat>;
	p_ct = new fvkCameraThread(b, _device_id, _resolution);
	p_pt = new fvkCameraProcessingThread(b, this, _device_id);
}
fvkCamera::fvkCamera(const std::string& _video_file, cv::Size _resolution, int _api) :
fvkCameraAbstract(),
p_stdct(nullptr),
p_stdpt(nullptr)
{
	fvkSemaphoreBuffer<cv::Mat>* b = new fvkSemaphoreBuffer<cv::Mat>;
	p_ct = new fvkCameraThread(b, _video_file, _resolution, _api);
	p_pt = new fvkCameraProcessingThread(b, this, 0);
}

fvkCamera::fvkCamera(fvkCameraThread* _ct, fvkCameraProcessingThread* _pt) :
fvkCameraAbstract(),
p_stdct(nullptr),
p_stdpt(nullptr),
p_ct(nullptr),
p_pt(nullptr)
{
	//if (_ct && _pt)
	//{
		//fvkSemaphoreBuffer<cv::Mat>* b = new fvkSemaphoreBuffer<cv::Mat>;
		//_ct->setSemaphoreBuffer(b);
		//_pt->setSemaphoreBuffer(b);
		p_ct = _ct;
		p_pt = _pt;
	//}
}

fvkCamera::~fvkCamera()
{
	if (disconnect())
	{
		if (p_pt)
		{
			delete p_pt;
			p_pt = nullptr;
		}
		if (p_ct)
		{
			delete p_ct;
			p_ct = nullptr;
		}
	}
}
bool fvkCamera::disconnect()
{
	// first stop the processing thread.
	if (p_pt)
	{
		p_pt->stop();
		std::cout << "Camera processing thread has been successfully stopped.\n";
	}

	// then stop the camera thread and release/close the device.
	if (p_ct)
	{
		if (p_ct->isOpened())
		{
			p_ct->stop();
			if (p_ct->close())
			{
				std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been successfully disconnected.\n";
				return true;
			}
		}
		std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been disconnected already.\n";
	}
	return false;
}
bool fvkCamera::connect()
{
	if (p_ct)
	{
		if (p_ct->isOpened())
		{
			std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been connected already.\n";
			return false;
		}

		return p_ct->open();
	}
	std::cout << "Could not open the camera or video device!\n";
	return false;
}
bool fvkCamera::start()
{
	if (p_ct == nullptr) return false;

	if (p_ct->isOpened())
	{
		// Run some task on new thread. The launch policy std::launch::async
		// makes sure that the task is run asynchronously on a new thread.
		//auto future1 = std::async(std::launch::async, [&]
		//{
		//	p_ct->run();
		//});

		//future1.wait_for(std::chrono::milliseconds(1000)); // wait for camera thread to start, then start the processing thread.
		//
		//auto future2 = std::async(std::launch::async, [&]
		//{
		//	p_pt->run();
		//});

		if (p_stdct) delete p_stdct;
		p_stdct = new std::thread([&] { p_ct->run(); });
		p_stdct->detach();
		if (p_stdpt) delete p_stdpt;
		p_stdpt = new std::thread([&] { p_pt->run(); });
		p_stdpt->detach();

		return true;
	}
	return false;
}
bool fvkCamera::isConnected()
{
	if (p_ct)
		return p_ct->isOpened();
	return false;
}
void fvkCamera::pause(bool _b) { if (p_ct) p_ct->pause(_b); }
bool fvkCamera::pause() { if (p_ct) return p_ct->pause(); return false; }
void fvkCamera::repeat(bool _b) { if (p_ct) p_ct->repeat(_b); }
bool fvkCamera::repeat() { if (p_ct) return p_ct->repeat(); return true; }
cv::Mat fvkCamera::getFrame() const { if (p_pt) return p_pt->getFrame(); return cv::Mat(); }

void fvkCamera::saveFrameOnClick()
{
	if (p_pt) p_pt->saveFrameOnClick();
}
void fvkCamera::setFrameOutputLocation(const std::string& _filename)
{
	if (p_pt) p_pt->setFrameOutputLocation(_filename);
}
std::string fvkCamera::getFrameOutputLocation() const
{
	if (p_pt) return p_pt->getFrameOutputLocation();
	return "";
}
void fvkCamera::setVideoFile(const std::string& _filename)
{
	if (p_ct) p_ct->setVideoFile(_filename);
}
std::string fvkCamera::getVideoFile() const
{
	if (p_ct) return p_ct->getVideoFile();
	return "";
}

void fvkCamera::setDeviceIndex(int _index)
{
	if (p_ct) p_ct->setDeviceIndex(_index);
}
int fvkCamera::getDeviceIndex() const
{
	if (p_ct) return p_ct->getDeviceIndex();
	return 0;
}

void fvkCamera::setFrameDelay(int _delay_msec)
{
	if (p_ct) p_ct->setFrameDelay(_delay_msec);
}
int fvkCamera::getFrameDelay() const
{
	if (p_ct) return p_ct->getFrameDelay();
	return 33;
}
void fvkCamera::setSyncEnabled(bool _b)
{
	if (p_ct) p_ct->setSyncEnabled(_b);
}
bool fvkCamera::isSyncEnabled()
{
	if (p_ct) return p_ct->isSyncEnabled();
	return false;
}


void fvkCamera::setFrameViewerSlot(const std::function<void(const cv::Mat&)>& _f)
{ 
	if (p_pt) p_pt->setFrameViewerSlot(_f); 
}
void fvkCamera::setFrameStatisticsSlot(const std::function<void(const fvkAverageFpsStats&)>& _f)
{ 
	if (p_pt) p_pt->setFrameStatisticsSlot(_f);
}
int fvkCamera::getAvgFps() const
{
	if (p_pt) return p_pt->getAvgFps();
	return 0;
}
int fvkCamera::getNFrames() const
{
	if (p_pt) return p_pt->getNFrames();
	return 0;
}
fvkVideoWriter* fvkCamera::getRecorder() const
{
	if (p_pt) return p_pt->getRecorder();
	return nullptr;
}
fvkCameraImageProcessing* fvkCamera::imageProcessing() const
{ 
	if (p_pt) return p_pt->imageProcessing();
	return nullptr;
}

void fvkCamera::processFrame(cv::Mat& _frame)
{
	//cv::cvtColor(_frame, _frame, CV_BGR2GRAY);
}

/************************************************************************/
/* Set Camera Settings                                                  */
/************************************************************************/
void fvkCamera::openDriverConfigDialog()
{
	if (p_ct) if (p_ct->isOpened()) p_ct->openDriverConfigDialog();
}
void fvkCamera::setMsec(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setMsec(val);
}
void fvkCamera::setPosFrames(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setPosFrames(val);
}

void fvkCamera::setSharpness(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setSharpness(val);
}
void fvkCamera::setAutoExposure(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setAutoExposure(val);
}
void fvkCamera::setFps(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setFps(val);
}
void fvkCamera::setFrameCount(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setFrameCount(val);
}

void fvkCamera::setWhiteBalanceBlueU(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setWhiteBalanceBlueU(val);
}
void fvkCamera::setWhiteBalanceRedV(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setWhiteBalanceRedV(val);
}
void fvkCamera::setFourCC(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setFourCC(val);
}
void fvkCamera::setConvertToRGB(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setConvertToRGB(val);
}
void fvkCamera::setBrightness(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setBrightness(val);
}
void fvkCamera::setContrast(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setContrast(val);
}
void fvkCamera::setSaturation(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setSaturation(val);
}
void fvkCamera::setHue(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setHue(val);
}
void fvkCamera::setGain(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setGain(val);
}
void fvkCamera::setExposure(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setExposure(val);
}
void fvkCamera::setRectification(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setRectification(val);
}
void fvkCamera::setFormat(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setFormat(val);
}
void fvkCamera::setMode(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setMode(val);
}

void fvkCamera::setGamma(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setGamma(val);
}
void fvkCamera::setTemperature(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setTemperature(val);
}
void fvkCamera::setZoom(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setZoom(val);
}
void fvkCamera::setFocus(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setFocus(val);
}
void fvkCamera::setIsoSpeed(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setIsoSpeed(val);
}
void fvkCamera::setBackLight(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setBackLight(val);
}
void fvkCamera::setPan(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setPan(val);
}
void fvkCamera::setTilt(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setTilt(val);
}
void fvkCamera::setRoll(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setRoll(val);
}
void fvkCamera::setTrigger(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setTrigger(val);
}
void fvkCamera::setTriggerDelay(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setTriggerDelay(val);
}
void fvkCamera::setAviRatio(double val)
{
	if (p_ct) if (p_ct->isOpened()) p_ct->setAviRatio(val);
}
void fvkCamera::setResolution(cv::Size _res)
{
	if (p_ct) p_ct->setResolution(_res);
}
/************************************************************************/
/* Get Camera Settings                                                  */
/************************************************************************/
cv::Size fvkCamera::getResolution() const
{
	if (p_ct) return p_ct->getResolution();
	return cv::Size(0, 0);
}
void fvkCamera::setWidth(int _w)
{
	if (p_ct) p_ct->setWidth(_w);
}
int fvkCamera::getWidth() const
{
	if (p_ct) return p_ct->getWidth();
	return 0;
}
void fvkCamera::setHeight(int _h)
{
	if (p_ct) p_ct->setHeight(_h);
}
int fvkCamera::getHeight() const
{
	if (p_ct) return p_ct->getHeight();
	return 0;
}
double fvkCamera::getSharpness() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getSharpness();
	return 0;
}
double fvkCamera::getAutoExposure() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getAutoExposure();
	return 0;
}
double fvkCamera::getFps() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getFps();
	return 0;
}
double fvkCamera::getFrameCount() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getFrameCount();
	return 0;
}
double fvkCamera::getWhiteBalanceBlueU() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getWhiteBalanceBlueU();
	return 0;
}
double fvkCamera::getWhiteBalanceRedV() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getWhiteBalanceRedV();
	return 0;
}
std::string fvkCamera::getFourCC() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getFourCC();
	return "";
}

double fvkCamera::getConvertToRGB() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getConvertToRGB();
	return 0;
}
double fvkCamera::getBrightness() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getBrightness();
	return 0;
}
double fvkCamera::getContrast() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getContrast();
	return 0;
}
double fvkCamera::getSaturation() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getSaturation();
	return 0;
}
double fvkCamera::getHue() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getHue();
	return 0;
}
double fvkCamera::getGain() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getGain();
	return 0;
}
double fvkCamera::getExposure() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getExposure();
	return 0;
}
double fvkCamera::getRectification() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getRectification();
	return 0;
}
double fvkCamera::getFormat() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getFormat();
	return 0;
}
double fvkCamera::getMode() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getMode();
	return 0;
}
double fvkCamera::getGamma() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getGamma();
	return 0;
}
double fvkCamera::getTemperature() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getTemperature();
	return 0;
}
double fvkCamera::getZoom() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getZoom();
	return 0;
}
double fvkCamera::getFocus() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getFocus();
	return 0;
}
double fvkCamera::getIsoSpeed() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getIsoSpeed();
	return 0;
}
double fvkCamera::getBackLight() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getBackLight();
	return 0;
}
double fvkCamera::getPan() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getPan();
	return 0;
}
double fvkCamera::getTilt() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getTilt();
	return 0;
}
double fvkCamera::getRoll() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getRoll();
	return 0;
}
double fvkCamera::getTrigger() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getTrigger();
	return 0;
}
double fvkCamera::getTriggerDelay() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getTriggerDelay();
	return 0;
}
double fvkCamera::getMsec() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getMsec();
	return 0;
}
double fvkCamera::getPosFrames() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getPosFrames();
	return 0;
}
double fvkCamera::getAviRatio() const
{
	if (p_ct) if (p_ct->isOpened()) return p_ct->getAviRatio();
	return 0;
}