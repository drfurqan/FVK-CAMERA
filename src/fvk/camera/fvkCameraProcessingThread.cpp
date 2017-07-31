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

fvkCameraProcessingThread::fvkCameraProcessingThread(int _device_index, fvkSemaphoreBuffer<cv::Mat>* _buffer, fvkCameraAbstract* _frameobserver) :
m_device_index(_device_index),
p_buffer(_buffer),
p_frameobserver(_frameobserver),
p_ip(new fvkCameraImageProcessing()),
p_vr(new fvkVideoWriter()),
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
	if (p_vr)
	{
		p_vr->stop();
		delete p_vr;
		p_vr = nullptr;
	}
	if (p_ip) delete p_ip;
	p_ip = nullptr;
	std::cout << "Camera processing thread has been stopped.\n";
}

void fvkCameraProcessingThread::run()
{
	while (1)
	{
		// stop this thread.
		m_stopmutex.lock();
		if (m_isstop)
		{
			if (p_vr) p_vr->stop();
			m_isstop = false;
			m_stopmutex.unlock();
			break;
		}
		m_stopmutex.unlock();

		// get a frame from the camera thread.
		m_frame = p_buffer->get();

		// do some basic image processing
		if (p_ip) p_ip->imageProcessing(m_frame);

		// send frame to the observer to process it on another class.
		if (p_frameobserver)
			p_frameobserver->processFrame(m_frame);

		// emit signal to inform to image box for the new frame.
		if (emit_display_frame)
			emit_display_frame(m_frame);

		// save current frame to disk.
		saveFrameToDisk(m_frame);

		// add frame for the video recording.
		if (p_vr) p_vr->addFrame(m_frame);

		// update stats.
		m_statsmutex.lock();
		m_avgfps.update();
		m_statsmutex.unlock();
		if (emit_stats)
			emit_stats(m_avgfps.stats);
	}

	if (p_vr) p_vr->stop();
	std::cout << "Stopping camera processing thread...";
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

static std::string __extractFileExt(const std::string& _str)
{
	std::string result = "";
	size_t extPos = _str.rfind(".");
	if (extPos != std::string::npos)
		result = _str.substr(extPos);
	result.erase(0, 1);	// remove the first dot.
	return result;
}

static std::string __extractFilePath(const std::string& _str)
{
	std::string result = "";
	size_t pathPos = _str.rfind("/");
	if (pathPos == std::string::npos)
		pathPos = _str.rfind("\\");
	if (pathPos != std::string::npos)
		result = _str.substr(0, pathPos);
	return result;
}
static std::string __changeFileName(const std::string& _path, const std::string& _new_name)
{
#if defined(_WIN32)
	return __extractFilePath(_path) + "\\" + _new_name + "." + __extractFileExt(_path);
#else 
	return __extractFilePath(_path) + "/" + _new_name + "." + __extractFileExt(_path);
#endif
}
static std::string __extractFileNameWithExt(const std::string &aString)
{
	std::string result = "";
	size_t pathPos = aString.rfind("/");
	if (pathPos == std::string::npos)
		pathPos = aString.rfind("\\");
	if (pathPos != std::string::npos)
		result = aString.substr(pathPos + 1);
	return result;
}
static std::string __extractFileNameWithoutExt(const std::string &filepath)
{
	std::string aString = __extractFileNameWithExt(filepath);
	size_t position = aString.find(".");
	std::string result = (std::string::npos == position) ? aString : aString.substr(0, position);
	return result;
}

bool fvkCameraProcessingThread::saveFrameToDisk(const cv::Mat& _frame)
{
	m_savemutex.lock();
	bool b = false;
	if (m_iscapture)
	{
		std::string _path(m_filelocation);
		std::string n = __extractFileNameWithoutExt(_path) + "_" + std::to_string(m_device_index) + "_" + std::to_string(m_savedframes++);
		std::string p = __changeFileName(_path, n);
		b = cv::imwrite(p, m_frame);
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