/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2017/02/09   11:41PM
filename: 	fvkCameraThread.cpp
file base:	fvkCameraThread
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Class to create a thread for capturing frames from OpenCV camera.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkCameraThread.h>
#include <thread>
#include <chrono>

using namespace R3D;

fvkCameraThread::fvkCameraThread(fvkSemaphoreBuffer<cv::Mat>* _buffer, int _device_index, cv::Size _resolution) :
fvkCameraThreadAbstract(),
p_buffer(_buffer),
m_sync_proc_thread(false),
m_isrepeat(true)
{
	setDeviceIndex(_device_index);
	setResolution(_resolution);
	setDelay(33);	// frame delay for the camera device.
}
fvkCameraThread::fvkCameraThread(fvkSemaphoreBuffer<cv::Mat>* _buffer, const std::string& _video_file, cv::Size _resolution, int _api) :
fvkCameraThreadAbstract(),
p_buffer(_buffer),
m_sync_proc_thread(false),
m_isrepeat(true)
{
	setVideoFile(_video_file);
	setResolution(_resolution);
	setAPI(_api);
	setDelay(static_cast<int>(1000.0 / getFps()));	// frame delay of the opened video.
}
fvkCameraThread::~fvkCameraThread()
{
	stop();
	close();
#ifdef _DEBUG
	std::cout << "Camera thread has been stopped.\n";
#endif // _DEBUG
}

bool fvkCameraThread::grab(cv::Mat& _m_frame)
{
	// grab from the video file.
	if (!m_filepath.empty())					// if there is a *.avi video file, then grab from it.
	{
		if (!p_cam.grab())						// capture frame (if available).
		{
			m_repeatmutex.lock();
			if (m_isrepeat)
			{
				setPosFrames(0);				// reset the camera frame to 0.
				resetStats();					// reset frame counter as well.
				m_repeatmutex.unlock();
				return false;
			}
			m_repeatmutex.unlock();
		}
	}
	else										// otherwise, grab from the camera device.
	{
		if (!p_cam.grab())						// capture frame (if available).
			return false;
	}
	
	return p_cam.retrieve(_m_frame);
}

void fvkCameraThread::run()
{
	if (grab(m_frame))
	{
		m_syncmutex.lock();
		p_buffer->put(m_frame, m_sync_proc_thread);
		m_syncmutex.unlock();
	}
	else
	{
		#ifdef _DEBUG
		std::cout << "Camera # " << m_device_index << " could not grab the frame.\n";
		#endif // _DEBUG
	}

	if (emit_stats)
		emit_stats(m_avgfps.getStats());
}

cv::Mat fvkCameraThread::getFrame()
{
	return p_buffer->get().clone();
}

void fvkCameraThread::repeat(bool _b)
{
	std::lock_guard<std::mutex> lk(m_repeatmutex);
	m_isrepeat = _b;
}
auto fvkCameraThread::repeat() -> bool
{
	std::lock_guard<std::mutex> lk(m_repeatmutex);
	return m_isrepeat;
}

void fvkCameraThread::setSyncEnabled(bool _b)
{
	std::lock_guard<std::mutex> lk(m_syncmutex);
	m_sync_proc_thread = _b;
}
auto fvkCameraThread::isSyncEnabled() -> bool
{
	std::lock_guard<std::mutex> locker(m_syncmutex);
	return m_sync_proc_thread;
}
