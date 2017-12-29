/*********************************************************************************
created:	2016/01/10   01:37AM
modified:	2017/02/09   11:48PM
filename: 	fvkCameraProcessingThread.cpp
file base:	fvkCameraProcessingThread
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Class to create a thread for processing of the grabbed camera frame.
This thread is synchronized with the camera thread using a semaphore.

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

fvkCameraProcessingThread::fvkCameraProcessingThread(fvkSemaphoreBuffer<cv::Mat>* _buffer, fvkCameraAbstract* _frameobserver, int _device_index) :
p_buffer(_buffer),
p_frameobserver(_frameobserver),
m_device_index(_device_index),
m_frame(cv::Mat()),
m_rect(cv::Rect(0, 0, 50, 50)),
m_filelocation("D:\\saved_snapshot.jpg"),
m_savedframes(0),
m_iscapture(false)
{
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
	if (emit_display_frame)
		emit_display_frame(m_frame);

	// save current frame to disk.
	saveFrameToDisk(m_frame);

	// add frame for the video recording.
	if (m_vr.isOpened())
		m_vr.addFrame(m_frame);

	if (emit_stats)
		emit_stats(m_avgfps.getStats());
}
cv::Mat fvkCameraProcessingThread::getFrame()
{
	return p_buffer->get().clone();
}

void fvkCameraProcessingThread::saveFrameOnClick()
{
	std::unique_lock<std::mutex> locker(m_savemutex);
	m_iscapture = true;
}

auto fvkCameraProcessingThread::saveFrameToDisk(const cv::Mat& _frame) -> bool
{
	m_savemutex.lock();
	auto b = false;
	if (m_iscapture)
	{
		std::vector<int> params;
		params.push_back(cv::IMWRITE_JPEG_QUALITY);
		params.push_back(98);   // that's percent, so 100 == no compression, 1 == full.
		b = cv::imwrite(m_filelocation, _frame, params);
		m_iscapture = false;
	}
	m_savemutex.unlock();
	return b;
}