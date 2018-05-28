#pragma once
#ifndef fvkThread_h__
#define fvkThread_h__

/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2017/02/09   12:03AM
filename: 	fvkThread.h
file base:	fvkThread
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class for a standard thread with thread safe operations such as pause,
stop, delay, getting average FPS, and total number of processed frames, etc. This 
class can be used in two ways, either you derive a new class from this class and 
override the run() function, or you just specify the function you want to call in 
the thread in the start() function.

usage example # 1:
------------------

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
------------------

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

#include "fvkExport.h"
#include "fvkAverageFps.h"

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace R3D
{

class FVK_EXPORT fvkThread
{

public:
	// Description:
	// Default constructor to create and initializes the data.
	// Default thread delay is 30 milliseconds.
	fvkThread();
	// Description:
	// Default destructor.
	virtual ~fvkThread() = default;

	// Description:
	// A thread function (functor) to be executed by the thread.
	void operator()(std::function<void()> _func = nullptr);

	// Description:
	// Virtual function that is expected to be overridden in the derived class.
	// If a function (_func) is specified in start() function, then the specified function will be
	// called by the thread, otherwise this overridden function will be called.
	virtual void run();

	// Description:
	// Function to start this thread.
	// If a function (_func) is specified, then the specified function will be
	// called by the thread, otherwise the overridden function run() will be
	// called.
	// This function will be called by the thread function (functor).
	void start(std::function<void()> _func = nullptr);
	// Description:
	// Function to pause (true) or resume (false) this thread.
	void pause(const bool _b);
	// Description:
	//Function that returns true if this thread is on pause state.
	auto pause() -> bool;

	// Description:
	// Function to stop this thread.
	void stop();
	// Description:
	// Function that returns true if the thread is active or in running mode.
	auto active() const -> bool;

	// Description:
	// Function to set the time delay in milliseconds which makes 
	// delay this thread for the specified time.
	// Default delay is 30 milliseconds.
	void setDelay(const int _delay_msec);
	// Description:
	// Function to get the time delay in milliseconds.
	auto getDelay() -> int;

	// Description:
	// Function that returns the average frames per second of this thread.
	auto getAvgFps() -> int;

	// Description:
	// Function that returns the total number of processed or passed frames.
	void setFrameNumber(const int _frame);
	// Description:
	// Function that returns the total number of processed or passed frames.
	auto getFrameNumber() -> int;

	// Description:
	// Function that sleeps a thread for specified time (a thread in which this function is called).
	static void sleep(const unsigned long _milliseconds);
	// Description:
	// Function that sleeps a thread till specified time point (a thread in which this function is called).
	static void sleep_until(const unsigned long _milliseconds);

protected:
	fvkAverageFps m_avgfps;

private:
	std::mutex m_statsmutex;
	std::mutex m_pausemutex;
	std::condition_variable m_pausecond;
	std::atomic<bool> m_isstop;
	bool m_ispause;
	int m_delay;
};

}

#endif // fvkThread_h__