/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2017/02/09   11:41PM
filename: 	fvkCameraThread.cpp
file base:	fvkCameraThread
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class to create a camera capturing thread. This class can be further
extended in order to make any camera device compatible to this library.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkCameraThread.h>

using namespace R3D;

fvkCameraThread::fvkCameraThread(const int _device_index, const cv::Size& _frame_size, fvkSemaphoreBuffer<cv::Mat>* _buffer) :
	fvkThread(),
	fvkCameraThreadAbstract(),
	p_buffer(_buffer),
	m_video_output_func(nullptr),
	m_sync_proc_thread(false),
	m_rect(cv::Rect(0, 0, 10, 10))
{
	setDeviceIndex(_device_index);
	setFrameSize(_frame_size);
	setDelay(1000 / 33);	// delay between frames (30 fps).
}

void fvkCameraThread::run()
{
	if (!p_buffer)
		return;

	cv::Mat f;

	if (grab(f))
	{
		m_rectmutex.lock();
		const auto r = m_rect;
		m_rectmutex.unlock();

		if (r.width > f.cols || r.height > f.rows)
			return;

		auto frame = cv::Mat(f, r);

		p_buffer->put(frame, m_sync_proc_thread);

		// emit signal to inform to image box for the new frame.
		if (m_video_output_func)
			m_video_output_func(frame, m_avgfps.getStats());
	}
	else
	{
		#ifdef _DEBUG
		std::cout << "Camera # " << m_device_index << " could not grab the frame.\n";
		#endif // _DEBUG
	}
}

void fvkCameraThread::setVideoOutput(const std::function<void(cv::Mat&, const fvkThreadStats&)> _f)
{
	m_video_output_func = std::move(_f);
}

auto fvkCameraThread::getFrame() -> cv::Mat
{
	const auto f = p_buffer->get();
	if (f.empty())
		return cv::Mat();

	m_rectmutex.lock();
	const auto r = m_rect;
	m_rectmutex.unlock();

	if (r.width > f.cols || r.height > f.rows)
		return cv::Mat();

	return cv::Mat(f.clone(), r);
}
void fvkCameraThread::setRoi(const cv::Rect& _roi)
{
	std::lock_guard<std::mutex> locker(m_rectmutex);
	m_rect = _roi;
}
auto fvkCameraThread::getRoi() -> cv::Rect
{
	std::lock_guard<std::mutex> locker(m_rectmutex);
	return m_rect;
}
void fvkCameraThread::setSyncEnabled(const bool _b)
{
	m_sync_proc_thread = _b;
}
auto fvkCameraThread::isSyncEnabled() const -> bool
{
	return m_sync_proc_thread;
}