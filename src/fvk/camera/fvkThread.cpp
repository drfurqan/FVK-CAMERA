/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2017/02/09   12:03AM
filename: 	fvkThread.cpp
file base:	fvkThread
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class for a standard thread with thread safe operations such as pause,
stop, delay, getting average FPS, and total number of processed frames, etc. This
class can be used in two ways, either you derive a new class from this class and
override the run() function, or you just specify a function you want to call in
the start() function.

usage example # 1:
class MyTask : public fvkThread
{
public:
void run() override				// overridden function.
{ std::cout << "running 1..." << std::endl; }
};
std::thread th([&]()			// creating a thread with lamda to execute our task
{
task.start();
});
// OR
std::thread th(std::ref(task));

usage example # 2:
void any_function()
{
std::cout << "running 2..." << std::endl;
}
std::thread th([&]()			// creating a thread with lamda to execute our task
{
task.start(any_function);	// now any_function will be called
});
// OR
std::thread th(std::ref(task), any_function);

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkThread.h>
#include <iostream>
#include <thread>

using namespace R3D;

fvkThread::fvkThread() :
	m_isstop(false),
	m_ispause(false),
	m_delay(1000 / 33)	// delay between frames (30 fps).
{
}

void fvkThread::operator()(const std::function<void()> func)
{
	start(func);
}

void fvkThread::run()
{
	// expected to be overridden...
}

void fvkThread::start(const std::function<void()> func)
{
	// make stats to zero for the new run.
	m_avgfps.getStats().nfps = 0;
	m_avgfps.getStats().nframes = 0;
	m_isstop = false;

	// start the main thread.
	while (true)
	{
		// stop this thread.
		if (m_isstop)
		{
			m_isstop = false;
			break;
		}

		// pause this thread.
		while (m_ispause)
		{
			std::unique_lock<std::mutex> lk(m_pausemutex);
			m_pausecond.wait(lk);
			lk.unlock();
		}

		if (func)
			func();
		else
			run();	// function to be overridden

		// update stats.
		m_statsmutex.lock();
		m_avgfps.update();
		sleep(m_delay);
		m_statsmutex.unlock();
	}
}

void fvkThread::stop()
{
	m_isstop = true;
}
auto fvkThread::active() const -> bool
{
	return !m_isstop;
}

void fvkThread::pause(const bool b)
{
	{
		std::lock_guard<std::mutex> lk(m_pausemutex);
		m_ispause = b;
	}
	m_pausecond.notify_one();
}
auto fvkThread::pause() -> bool
{
	std::lock_guard<std::mutex> lk(m_pausemutex);
	return m_ispause;
}

auto fvkThread::getAvgFps() -> int
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	return m_avgfps.getStats().nfps;
}
void fvkThread::setFrameNumber(const int frame)
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	m_avgfps.getStats().nframes = frame;
}
auto fvkThread::getFrameNumber() -> int
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	return m_avgfps.getStats().nframes;
}

void fvkThread::setDelay(const int delay_msec)
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	m_delay = delay_msec;
}
auto fvkThread::getDelay() -> int
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	return m_delay;
}

void fvkThread::sleep(const unsigned long milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
void fvkThread::sleep_until(const unsigned long milliseconds)
{
	std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(milliseconds));
}