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

fvkCameraThread::fvkCameraThread(int _device_index, fvkSemaphoreBuffer<cv::Mat>* _buffer, cv::Size _resolution) :
fvkCameraThreadAbstract(),
p_buffer(_buffer),
m_sync_proc_thread(false)
{
	m_device_index = _device_index;
	m_width = _resolution.width;
	m_height = _resolution.height;
}
fvkCameraThread::fvkCameraThread(const std::string& _video_file, fvkSemaphoreBuffer<cv::Mat>* _buffer, cv::Size _resolution) :
fvkCameraThreadAbstract(),
p_buffer(_buffer),
m_sync_proc_thread(false)
{
	m_filepath = _video_file;
	m_device_index = -1;
	m_width = _resolution.width;
	m_height = _resolution.height;
}
fvkCameraThread::~fvkCameraThread()
{
	stop();
	close();
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
				m_statsmutex.lock();
				m_avgfps.stats.nframes = 0;		// reset frame counter as well.
				m_statsmutex.unlock();
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
	while (1)
	{
		// stop this thread.
		m_stopmutex.lock();
		if (m_isstop)
		{
			m_isstop = false;
			m_stopmutex.unlock();
			break;
		}
		m_stopmutex.unlock();

		// pause this thread.
		while (m_ispause)
		{
			std::unique_lock<std::mutex> lk(m_pausemutex);
			m_pausecond.wait(lk);
			lk.unlock();
		}

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

		// update stats.
		m_statsmutex.lock();
		m_avgfps.update();
		m_statsmutex.unlock();
		if (emit_stats)
			emit_stats(m_avgfps.stats);

		std::this_thread::sleep_for(std::chrono::milliseconds(m_frame_msec));		// delay in frames.
	}

	std::cout << "Stopping camera thread...";
}
void fvkCameraThread::stop()
{
	std::lock_guard<std::mutex> ml(m_stopmutex);
	m_isstop = true;
}
void fvkCameraThread::pause(bool _b)
{
	std::lock_guard<std::mutex> ml(m_pausemutex);
	if (_b)
	{
		m_ispause = true;
	}
	else
	{
		m_ispause = false;
		m_pausecond.notify_one();
	}
}
bool fvkCameraThread::pause()
{
	std::lock_guard<std::mutex> lk(m_pausemutex);
	return m_ispause;
}
void fvkCameraThread::repeat(bool _b)
{
	std::lock_guard<std::mutex> lk(m_repeatmutex);
	m_isrepeat = _b;
}
bool fvkCameraThread::repeat()
{
	std::lock_guard<std::mutex> lk(m_repeatmutex);
	return m_isrepeat;
}
int fvkCameraThread::getAvgFps()
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	return m_avgfps.stats.fps;
}
int fvkCameraThread::getNFrames()
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	return m_avgfps.stats.nframes;
}
cv::Mat fvkCameraThread::getFrame()
{
	return p_buffer->get().clone();
}
void fvkCameraThread::setSyncEnabled(bool _b)
{
	std::lock_guard<std::mutex> lk(m_syncmutex);
	m_sync_proc_thread = _b;
}
bool fvkCameraThread::isSyncEnabled()
{
	std::lock_guard<std::mutex> locker(m_syncmutex);
	return m_sync_proc_thread;
}
