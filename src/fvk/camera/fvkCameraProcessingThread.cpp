/*********************************************************************************
created:	2016/01/10   01:37AM
modified:	2017/02/09   11:48PM
filename: 	fvkCameraProcessingThread.cpp
file base:	fvkCameraProcessingThread
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

#include <fvk/camera/fvkCameraProcessingThread.h>
#include <fvk/camera/fvkCamera.h>

using namespace R3D;

fvkCameraProcessingThread::fvkCameraProcessingThread(const int _device_index, fvkCameraAbstract* _frameobserver, fvkSemaphoreBuffer<cv::Mat>* _buffer) :
	m_device_index(_device_index),
	p_frameobserver(_frameobserver),
	p_buffer(_buffer),
	m_frame(cv::Mat()),
	m_rect(cv::Rect(0, 0, 50, 50)),
	m_filepath("D:\\saved_snapshot.jpg"),
	m_save(false)
{
	// this thread is synchronized with the camera thread by semaphore buffer,
	// which means it is fully dependent on the camera thread, if a frame is
	// added to the buffer queue by the camera thread, only then it will run,
	// otherwise it will keep waiting for the next frame,
	// so, no need to make a thread delay manually.
	setDelay(0);
}

fvkCameraProcessingThread::~fvkCameraProcessingThread()
{
	stop();
	m_vr.stop();
#ifdef _DEBUG
	std::cout << "Camera processing thread has been stopped.\n";
#endif // _DEBUG
}

void fvkCameraProcessingThread::run()
{
	// get a frame from the camera thread.
	m_frame = p_buffer->get();

	// do some basic image processing
	m_ip.imageProcessing(m_frame);

	// send frame to the observer to process it on another class.
	if (p_frameobserver)
		p_frameobserver->processFrame(m_frame);

	// emit signal to inform to image box for the new frame.
	if (m_emit_display_frame)
		m_emit_display_frame(m_frame);

	// save current frame to disk.
	saveFrameToDisk(m_frame);

	// add frame for the video recording.
	if (m_vr.isOpened())
		m_vr.addFrame(m_frame);

	if (m_emit_stats)
		m_emit_stats(m_avgfps.getStats());
}
auto fvkCameraProcessingThread::getFrame() const -> cv::Mat
{
	return p_buffer->get().clone();
}

void fvkCameraProcessingThread::saveFrameOnClick()
{
	m_save = true;
}

auto fvkCameraProcessingThread::saveFrameToDisk(const cv::Mat& _frame) -> bool
{
	auto b = false;
	if (m_save)
	{
		std::vector<int> params;
		params.push_back(cv::IMWRITE_JPEG_QUALITY);
		params.push_back(98);   // that's percent, so 100 == no compression, 1 == full.
		b = cv::imwrite(m_filepath, _frame, params);
		m_save = false;
	}
	return b;
}