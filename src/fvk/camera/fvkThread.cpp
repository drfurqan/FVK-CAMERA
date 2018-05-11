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

using namespace R3D;

fvkThread::fvkThread() :
	m_isstop(false),
	m_delay(1000 / 33),	// delay between frames (30 fps).
	m_ispause(false)
{
}
fvkThread::~fvkThread()
{
	stop();
}

void fvkThread::operator()(const std::function<void()> _func)
{
	start(_func);
}

void fvkThread::run()
{
}

void fvkThread::start(const std::function<void()> _func)
{
	// make stats to zero for the new run.
	m_avgfps.getStats().nfps = 0;
	m_avgfps.getStats().nframes = 0;

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

		if (_func)
			_func();
		else
			run();	// function to be overridden

		// update stats.
		m_statsmutex.lock();
		m_avgfps.update();
		std::this_thread::sleep_for(std::chrono::milliseconds(m_delay));
		m_statsmutex.unlock();
	}

#ifdef _DEBUG
	std::cout << "The thread has been stopped successfully.\n";
#endif // _DEBUG
}

void fvkThread::stop()
{
	m_isstop = true;
}
auto fvkThread::active() const -> bool
{
	return m_isstop;
}

void fvkThread::pause(bool _b)
{
	std::lock_guard<std::mutex> lk(m_pausemutex);
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
auto fvkThread::getNFrames() -> int
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	return m_avgfps.getStats().nframes;
}
void fvkThread::resetStats()
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	m_avgfps.getStats().nframes = 0;
}

void fvkThread::setDelay(const int _delay_msec)
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	m_delay = _delay_msec;
}
auto fvkThread::getDelay() -> int
{
	std::lock_guard<std::mutex> lk(m_statsmutex);
	return m_delay;
}