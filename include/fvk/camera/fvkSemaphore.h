#pragma once
#ifndef fvkSemaphore_h__
#define fvkSemaphore_h__

/*********************************************************************************
created:	2017/02/09   01:37AM
filename: 	fvkSemaphore.h
file base:	fvkSemaphore
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Basic semaphore like QSemaphore functionalities.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkExport.h"

#include <condition_variable>
#include <mutex>

namespace R3D
{

class FVK_EXPORT fvkSemaphore
{
public:
	// Description:
	// Default constructor that takes count and constructs the object.
	explicit fvkSemaphore(int _count = 0);
	
	// Description:
	// Non-implemented.
	fvkSemaphore(const fvkSemaphore&) = delete;
	fvkSemaphore(fvkSemaphore&&) = delete;
	fvkSemaphore& operator=(const fvkSemaphore&) = delete;
	fvkSemaphore& operator=(fvkSemaphore&&) = delete;

	// Description:
	// It blocks the thread, until get notify by calling notify() method.
	void wait();
	// Description:
	// It does not block the thread, but wait until get notify by calling notify() method and returns true.
	auto try_wait() -> bool;
	// Description:
	// It notifies to wait() method to unblock the thread, and also
	// it notifies to try_wait() method to unblock as well and return true to proceed.
	void notify();

	template <typename _Rep, typename _Period>
	auto wait_for(const std::chrono::duration<_Rep, _Period>& _d)
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		auto finished = m_cv.wait_for(lock, _d, [&] { return m_count > 0; });
		if (finished)
			--m_count;
		return finished;
	}

	template <typename _Clock, typename _Duration>
	auto wait_until(const std::chrono::time_point<_Clock, _Duration>& _t)
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		auto finished = m_cv.wait_until(lock, _t, [&] { return m_count > 0; });
		if (finished)
			--m_count;
		return finished;
	}

	std::condition_variable::native_handle_type native_handle();

private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	int m_count;
};

}

#endif // fvkSemaphore_h__
