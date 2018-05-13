/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2018/05/07   12:03AM
filename: 	fvkCamera.cpp
file base:	fvkCamera
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Thread-safe multi-threaded camera class for any type of camera device.
			Capturing is done on one thread and captured frame processing on the other.
			Both threads are synchronized with semaphore methodology.

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

fvkCamera::fvkCamera(const int _device_index, const cv::Size& _frame_size) :
p_stdct(nullptr),
p_stdpt(nullptr)
{
	const auto b = new fvkSemaphoreBuffer<cv::Mat>();
	p_ct = new fvkCameraThreadOpenCV(_device_index, _frame_size, b);
	p_pt = new fvkCameraProcessingThread(_device_index, this, b);
}
fvkCamera::fvkCamera(const std::string& _video_file, const cv::Size& _frame_size, const int _api) :
p_stdct(nullptr),
p_stdpt(nullptr)
{
	const auto b = new fvkSemaphoreBuffer<cv::Mat>();
	p_ct = new fvkCameraThreadOpenCV(_video_file, _frame_size, _api, b);
	p_pt = new fvkCameraProcessingThread(p_ct->getDeviceIndex(), this, b);
}

fvkCamera::fvkCamera(fvkCameraThread* _ct) :
	p_stdct(nullptr),
	p_stdpt(nullptr)
{
	const auto b = new fvkSemaphoreBuffer<cv::Mat>();
	p_ct = _ct;
	p_ct->setSemaphoreBuffer(b);
	p_pt = new fvkCameraProcessingThread(_ct->getDeviceIndex(), this, b);
}

fvkCamera::fvkCamera(fvkCameraThread* _ct, fvkCameraProcessingThread* _pt) :
	p_stdct(nullptr),
	p_stdpt(nullptr),
	p_ct(_ct),
	p_pt(_pt)
{
	if(_ct->getSemaphoreBuffer() == nullptr && _pt->getSemaphoreBuffer() == nullptr)
	{
		const auto b = new fvkSemaphoreBuffer<cv::Mat>();
		p_ct->setSemaphoreBuffer(b);
		p_pt->setSemaphoreBuffer(b);
	}
}

fvkCamera::~fvkCamera()
{
	disconnect();

	if(p_stdct)
		delete p_stdct;
	p_stdct = nullptr;

	if (p_stdpt)
		delete p_stdpt;
	p_stdpt = nullptr;

	auto buffer = p_ct->getSemaphoreBuffer();
	if (buffer)
		delete buffer;
	buffer = nullptr;

	if (p_pt)
		delete p_pt;
	p_pt = nullptr;

	if (p_ct)
		delete p_ct;
	p_ct = nullptr;
}

auto fvkCamera::disconnect() -> bool
{
	if (p_ct)
	{
		// stop the main thread.
		if (p_ct->active())
		{
			p_ct->stop();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// release / close the device.
		if (p_ct->close())
			std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been disconnected successfully.\n";
		else
			std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been disconnected already.\n";
	}

	if (p_pt)
	{
		// stop the processing thread.
		if (p_ct->active())
		{
			p_pt->stop();
			p_pt->writer().stop();
			std::cout << "[" << p_pt->getDeviceIndex() << "] camera processing thread has been stopped successfully.\n";
		}
	}

	if (p_ct->isOpened())
	{
		std::cout << "[" << p_ct->getDeviceIndex() << "] camera is still running, couldn't disconnect!\n";
		return false;
	}

	return true;
}
auto fvkCamera::connect() -> bool
{
	if (!p_ct)
		return false;

	if (p_ct->isOpened())
	{
		std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been connected already.\n";
		return false;
	}

	const auto b = p_ct->open();

	// reset the region of interest.
	if (p_pt) 
		p_pt->setRoi(cv::Rect(0, 0, p_ct->getFrameWidth(), p_ct->getFrameHeight()));

	return b;
}
auto fvkCamera::start() -> bool
{
	if (!p_ct) 
		return false;

	if (!p_ct->isOpened())
		return false;

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

	if (p_stdct)
		delete p_stdct;
	p_stdct = new std::thread([&]() { p_ct->start(); });
	p_stdct->detach();

	if (p_stdpt)
		delete p_stdpt;
	p_stdpt = new std::thread([&]() { p_pt->start(); });
	p_stdpt->detach();

	return true;
}

auto fvkCamera::isConnected() const -> bool
{
	if (!p_ct) return false;
	return p_ct->isOpened();
}

void fvkCamera::pause(const bool _b) const
{
	if (!p_ct) return;
	p_ct->pause(_b);
}
auto fvkCamera::pause() const -> bool 
{ 
	if (!p_ct) return false;
	return p_ct->pause();
}
void fvkCamera::setDeviceIndex(const int _index) const
{
	if (!p_ct) return;
	p_ct->setDeviceIndex(_index);
}
auto fvkCamera::getDeviceIndex() const -> int
{
	if (!p_ct) return 0;
	return p_ct->getDeviceIndex();
}
void fvkCamera::setFrameSize(const cv::Size& _size) const
{
	if (!p_ct) return;
	p_ct->setFrameSize(_size);
}
auto fvkCamera::getFrameSize() const -> cv::Size
{
	if (!p_ct) return cv::Size(-1, -1);
	return p_ct->getFrameSize();
}
void fvkCamera::setDelay(const int _delay_msec) const
{
	if (!p_ct) return;
	p_ct->setDelay(_delay_msec);
}
auto fvkCamera::getDelay() const -> int
{
	if (!p_ct) return 0;
	return p_ct->getDelay();
}
void fvkCamera::setSyncEnabled(const bool _b) const
{
	if (!p_ct) return;
	p_ct->setSyncEnabled(_b);
}
auto fvkCamera::isSyncEnabled() const -> bool
{
	if (!p_ct) return false;
	return p_ct->isSyncEnabled();
}

auto fvkCamera::getFrame() const -> cv::Mat
{
	if (!p_pt) return cv::Mat();
	return p_pt->getFrame();
}
void fvkCamera::saveFrameOnClick() const
{
	if (!p_pt) return;
	p_pt->saveFrameOnClick();
}
void fvkCamera::setFrameOutputLocation(const std::string& _filename) const
{
	if (!p_pt) return;
	p_pt->setFrameOutputLocation(_filename);
}
auto fvkCamera::getFrameOutputLocation() const -> std::string
{
	if (!p_pt) return std::string();
	return p_pt->getFrameOutputLocation();
}
void fvkCamera::setFrameViewerSlot(const std::function<void(cv::Mat&)>& _f) const
{ 
	if (!p_pt) return;
	p_pt->setFrameViewerSlot(_f);
}
void fvkCamera::setFrameStatisticsSlot(const std::function<void(const fvkAverageFpsStats&)>& _f) const
{ 
	if (!p_pt) return;
	p_pt->setFrameStatisticsSlot(_f);
}
auto fvkCamera::getAvgFps() const -> int
{
	if (!p_pt) return 0;
	return p_pt->getAvgFps();
}
auto fvkCamera::getNFrames() const -> int
{
	if (!p_pt) return 0;
	return p_pt->getNFrames();
}

void fvkCamera::processFrame(cv::Mat& _frame)
{
	// override this function to do image processing on the captured "_frame".
}