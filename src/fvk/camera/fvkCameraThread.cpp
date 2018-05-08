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

using namespace R3D;

fvkCameraThread::fvkCameraThread(const int _device_index, const cv::Size& _frame_size, fvkSemaphoreBuffer<cv::Mat>* _buffer) :
	fvkThread(),
	fvkCameraThreadAbstract(),
	p_buffer(_buffer),
	m_sync_proc_thread(false)
{
	setDeviceIndex(_device_index);
	setFrameSize(_frame_size);
	setDelay(1000 / 33);	// delay between frames (30 fps).
}

fvkCameraThread::~fvkCameraThread()
{
}

void fvkCameraThread::run()
{
	if (grab(m_frame))
	{
		p_buffer->put(m_frame, m_sync_proc_thread);
	}
	else
	{
		#ifdef _DEBUG
		std::cout << "Camera # " << m_device_index << " could not grab the frame.\n";
		#endif // _DEBUG
	}

	if (m_emit_stats)
		m_emit_stats(m_avgfps.getStats());
}

auto fvkCameraThread::getFrame() const -> cv::Mat
{
	return p_buffer->get().clone();
}

void fvkCameraThread::setSyncEnabled(bool _b)
{
	m_sync_proc_thread = _b;
}
auto fvkCameraThread::isSyncEnabled() const -> bool
{
	return m_sync_proc_thread;
}