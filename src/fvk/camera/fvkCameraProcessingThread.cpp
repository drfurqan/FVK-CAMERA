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
m_isstop(false),
m_frame(cv::Mat()),
m_rect(cv::Rect(0, 0, 50, 50)),
m_filelocation("D:\\saved_snapshot.jpg"),
m_savedframes(0),
m_iscapture(false)
{
}

fvkCameraProcessingThread::~fvkCameraProcessingThread()
{
	stop();
	m_vr.stop();
	std::cout << "Camera processing thread has been stopped.\n";
}

void fvkCameraProcessingThread::run()
{
	// make stats to zero for the new run.
	m_avgfps.stats.fps = 0;
	m_avgfps.stats.nframes = 0;

	while (1)
	{
		// stop this thread.
		m_stopmutex.lock();
		if (m_isstop)
		{
			m_vr.stop();
			m_isstop = false;
			m_stopmutex.unlock();
			break;
		}
		m_stopmutex.unlock();

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

		// update stats.
		m_statsmutex.lock();
		m_avgfps.update();
		m_statsmutex.unlock();
		if (emit_stats)
			emit_stats(m_avgfps.stats);
	}

	m_vr.stop();
	std::cout << "Stopping camera processing thread...\n";
}
cv::Mat fvkCameraProcessingThread::getFrame()
{
	return p_buffer->get().clone();
}
void fvkCameraProcessingThread::stop()
{
	std::unique_lock<std::mutex> locker(m_stopmutex);
	m_isstop = true;
}

void fvkCameraProcessingThread::saveFrameOnClick()
{
	std::unique_lock<std::mutex> locker(m_savemutex);
	m_iscapture = true;
}

bool fvkCameraProcessingThread::saveFrameToDisk(const cv::Mat& _frame)
{
	m_savemutex.lock();
	bool b = false;
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

int fvkCameraProcessingThread::getAvgFps()
{
	std::unique_lock<std::mutex> locker(m_statsmutex);
	return m_avgfps.stats.fps;
}
int fvkCameraProcessingThread::getNFrames()
{
	std::unique_lock<std::mutex> locker(m_statsmutex);
	return m_avgfps.stats.nframes;
}