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
#include <fvk/camera/fvkProcessingThread.h>
//#include <future>

using namespace R3D;

fvkCamera::fvkCamera(const int device_index, const cv::Size& frame_size, const int api) :
	p_stdct(nullptr),
	p_stdpt(nullptr),
	m_ct_handle(nullptr),
	m_pt_handle(nullptr)
{
	const auto b = new fvkSemaphoreBuffer<cv::Mat>();
	p_ct = new fvkCameraThreadOpenCV(device_index, frame_size, api, b);
	p_pt = new fvkProcessingThread(device_index, this, b);
}
fvkCamera::fvkCamera(const std::string& video_file, const cv::Size& frame_size, const int api) :
	p_stdct(nullptr),
	p_stdpt(nullptr),
	m_ct_handle(nullptr),
	m_pt_handle(nullptr)
{
	const auto b = new fvkSemaphoreBuffer<cv::Mat>();
	p_ct = new fvkCameraThreadOpenCV(video_file, frame_size, api, b);
	p_pt = new fvkProcessingThread(p_ct->getDeviceIndex(), this, b);
}

fvkCamera::fvkCamera(fvkCameraThread* ct) :
	p_stdct(nullptr),
	p_stdpt(nullptr),
	m_ct_handle(nullptr),
	m_pt_handle(nullptr)
{
	const auto b = new fvkSemaphoreBuffer<cv::Mat>();
	p_ct = ct;
	p_ct->setSemaphoreBuffer(b);
	p_pt = new fvkProcessingThread(ct->getDeviceIndex(), this, b);
}

fvkCamera::fvkCamera(fvkCameraThread* ct, fvkProcessingThread* pt) :
	p_stdct(nullptr),
	p_stdpt(nullptr),
	m_ct_handle(nullptr),
	m_pt_handle(nullptr),
	p_ct(ct),
	p_pt(pt)
{
	if(ct->getSemaphoreBuffer() == nullptr && pt->getSemaphoreBuffer() == nullptr)
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

	if (p_pt)
		delete p_pt;
	p_pt = nullptr;

	if (p_ct)
	{
		auto buffer = p_ct->getSemaphoreBuffer();
		delete p_ct;

		if (buffer)
			delete buffer;
		buffer = nullptr;
	}
	p_ct = nullptr;
}

auto fvkCamera::disconnect() -> bool
{
	if (p_ct)
	{
		if (!p_ct->isOpened())
		{
			std::cout << "[" << p_ct->getDeviceIndex() << "] device needs to be connected first!\n";
			return false;
		}

		// stop the main thread.
		if (p_ct->active())
			p_ct->stop();

		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		// release / close the device.
		if (p_ct->close())
			std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been disconnected successfully.\n";
		else
			std::cout << "[" << p_ct->getDeviceIndex() << "] camera has been disconnected already.\n";
	}


	if (p_pt)
	{
		// stop the processing thread.
		if (p_pt->active())
		{
			p_pt->stop();
			p_pt->writer().stop();
			std::cout << "[" << p_pt->getDeviceIndex() << "] camera processing thread has been stopped successfully.\n";
		}
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

	// connect the device.
	const auto b = p_ct->open();

	// reset the region of interest.
	if (b)
	{
		p_ct->resetRoi();
	}

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
	//auto future1 = std::async(std::launch::deferred, [&]() { p_ct->start(); });
	//future1.wait_for(std::chrono::milliseconds(400)); // wait for camera thread to start, then start the processing thread.
	//auto future2 = std::async(std::launch::deferred, [&]() { p_pt->start(); });

	if (p_stdct)
		delete p_stdct;
	p_stdct = new std::thread([this]() { p_ct->start(); });
#if defined(_WIN32)
	m_ct_handle = p_stdct->native_handle();	// must save native handle before calling join() or detach().
#else
	m_ct_handle = nullptr;
#endif // _WIN32
	p_stdct->detach();

	if (p_stdpt)
		delete p_stdpt;
	p_stdpt = new std::thread([this]() { p_pt->start(); });
#if defined(_WIN32)
	m_pt_handle = p_stdpt->native_handle();	// must save native handle before calling join() or detach().
#else
	m_pt_handle = nullptr;
#endif // _WIN32
	p_stdpt->detach();

	return true;
}

auto fvkCamera::isConnected() const -> bool
{
	if (!p_ct) return false;
	return p_ct->isOpened();
}

void fvkCamera::pause(const bool b) const
{
	if (!p_ct) return;
	p_ct->pause(b);
}
auto fvkCamera::pause() const -> bool 
{ 
	if (!p_ct) return false;
	return p_ct->pause();
}
void fvkCamera::setDeviceIndex(const int index) const
{
	if (!p_ct) return;
	p_ct->setDeviceIndex(index);
}
auto fvkCamera::getDeviceIndex() const -> int
{
	if (!p_ct) return 0;
	return p_ct->getDeviceIndex();
}
void fvkCamera::setFrameSize(const cv::Size& size) const
{
	if (!p_ct) return;
	p_ct->setFrameSize(size);
}
auto fvkCamera::getFrameSize() const -> cv::Size
{
	if (!p_ct) return cv::Size(-1, -1);
	return p_ct->getFrameSize();
}
void fvkCamera::setDelay(const int delay_msec) const
{
	if (!p_ct) return;
	p_ct->setDelay(delay_msec);
}
auto fvkCamera::getDelay() const -> int
{
	if (!p_ct) return 0;
	return p_ct->getDelay();
}
void fvkCamera::setSyncEnabled(const bool b) const
{
	if (!p_ct) return;
	p_ct->setSyncEnabled(b);
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
void fvkCamera::setFrameOutputLocation(const std::string& filename) const
{
	if (!p_pt) return;
	p_pt->setFrameOutputLocation(filename);
}
auto fvkCamera::getFrameOutputLocation() const -> std::string
{
	if (!p_pt) return std::string();
	return p_pt->getFrameOutputLocation();
}
void fvkCamera::setVideoOutput(const std::function<void(cv::Mat&, const fvkThreadStats&)> f) const
{ 
	if (!p_pt) return;
	p_pt->setVideoOutput(f);
}
auto fvkCamera::getAvgFps() const -> int
{
	if (!p_pt) return 0;
	return p_pt->getAvgFps();
}
auto fvkCamera::getFrameNumber() const -> int
{
	if (!p_pt) return 0;
	return p_pt->getFrameNumber();
}
void fvkCamera::setVideoFileLocation(const std::string& filename) const
{
	const auto ocv = dynamic_cast<fvkCameraThreadOpenCV*>(p_ct);
	if (ocv) ocv->setVideoFileLocation(filename);
}
auto fvkCamera::getVideoFileLocation() const -> std::string
{
	const auto ocv = dynamic_cast<fvkCameraThreadOpenCV*>(p_ct);
	if (ocv) return ocv->getVideoFileLocation();
	return "";
}
void fvkCamera::repeat(const bool b) const
{
	const auto ocv = dynamic_cast<fvkCameraThreadOpenCV*>(p_ct);
	if (ocv) ocv->repeat(b);
}
auto fvkCamera::repeat() const -> bool
{
	const auto ocv = dynamic_cast<fvkCameraThreadOpenCV*>(p_ct);
	if (ocv) return ocv->repeat();
	return false;
}
void fvkCamera::setAPI(const int api) const
{
	const auto ocv = dynamic_cast<fvkCameraThreadOpenCV*>(p_ct);
	if (ocv) ocv->setAPI(api);
}
auto fvkCamera::getAPI() const -> int
{
	const auto ocv = dynamic_cast<fvkCameraThreadOpenCV*>(p_ct);
	if (ocv) return ocv->getAPI();
	return 0;
}
void fvkCamera::openConfigurationDialog()
{
	const auto ocv = dynamic_cast<fvkCameraThreadOpenCV*>(p_ct);
	if (ocv) ocv->openConfigurationDialog();
}

void fvkCamera::present(cv::Mat& frame)
{
	// override this function to do image processing on the captured "frame".
}
