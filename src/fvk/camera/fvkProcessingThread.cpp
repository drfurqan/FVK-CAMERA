/*********************************************************************************
created:	2016/01/10   01:37AM
modified:	2018/05/16   11:48PM
filename: 	fvkProcessingThread.cpp
file base:	fvkProcessingThread
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class that create a parallel thread for the processing of the grabbed
camera frame. This thread is synchronized with the camera thread using a semaphore.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkProcessingThread.h>
#include <fvk/camera/fvkCamera.h>

using namespace R3D;

fvkProcessingThread::fvkProcessingThread(const int _device_index, fvkCameraAbstract* _frameobserver, fvkSemaphoreBuffer<cv::Mat>* _buffer) :
	m_device_index(_device_index),
	p_frameobserver(_frameobserver),
	p_buffer(_buffer),
	m_filepath("D:\\saved_snapshot.jpg"),
	m_rect(cv::Rect(0, 0, 10, 10)),
	m_save(false)
{
	// this thread is synchronized with the camera thread by semaphore buffer,
	// which means it is fully dependent on the camera thread, if a frame is
	// added to the buffer queue by the camera thread, only then it will run,
	// otherwise it will keep waiting for the next frame,
	// so, no need to make a thread delay manually.
	setDelay(0);
}

fvkProcessingThread::fvkProcessingThread(const int _device_index, fvkSemaphoreBuffer<cv::Mat>* _buffer) : 
	fvkProcessingThread(_device_index, nullptr, _buffer)
{
}

fvkProcessingThread::~fvkProcessingThread()
{
	if(active())
	{
		stop();
		m_vr.stop();
	}
}

void fvkProcessingThread::run()
{
	if (!p_buffer)
		return;

	// get a frame from the camera buffer.
	const auto f = p_buffer->get();

	m_rectmutex.lock();
	const auto r = m_rect;
	m_rectmutex.unlock();

	if (r.width > f.cols || r.height > f.rows)
		return;

	auto frame = cv::Mat(f, r);

	// do some basic image processing
	m_ip.imageProcessing(frame);

	// send frame to the observer to process it on another class.
	if (p_frameobserver)
		p_frameobserver->present(frame);

	// expected to be overridden in the derived class.
	present(frame);

	// emit signal to inform to image box for the new frame.
	if (m_emit_func)
		m_emit_func(frame, m_avgfps.getStats());

	// save current frame to disk.
	saveFrameToDisk(frame);

	// add frame for the video recording.
	if (m_vr.isOpened())
		m_vr.addFrame(frame);
}

void fvkProcessingThread::setFrameViewerSlot(const std::function<void(cv::Mat&, const fvkThreadStats&)> _f)
{
	m_emit_func = std::move(_f);
}

void fvkProcessingThread::present(cv::Mat& _frame)
{
	// do nothing!
}

auto fvkProcessingThread::getFrame() -> cv::Mat
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
void fvkProcessingThread::setRoi(const cv::Rect& _roi)
{
	std::lock_guard<std::mutex> lock(m_rectmutex);
	m_rect = _roi;
}
auto fvkProcessingThread::getRoi() -> cv::Rect
{
	std::lock_guard<std::mutex> lock(m_rectmutex);
	return m_rect;
}

void fvkProcessingThread::saveFrameOnClick()
{
	m_save = true;
}
auto fvkProcessingThread::saveFrameToDisk(const cv::Mat& _frame) -> bool
{
	if (m_save)
	{
		m_save = false;
		std::vector<int> params;
		params.push_back(cv::IMWRITE_JPEG_QUALITY);
		params.push_back(98);   // that's percent, so 100 == no compression, 1 == fully compressed.
		return cv::imwrite(m_filepath, _frame, params);
	}
	return false;
}