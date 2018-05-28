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

purpose:	class for a basic semaphore like functionalities.

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
	explicit fvkSemaphore(const int _count = 0);
	
	// Description:
	// Non-implemented.
	fvkSemaphore(const fvkSemaphore&) = delete;
	fvkSemaphore(fvkSemaphore&&) = delete;
	fvkSemaphore& operator=(const fvkSemaphore&) = delete;
	fvkSemaphore& operator=(fvkSemaphore&&) = delete;

	// Description:
	// Function that blocks the thread, until get notify by calling notify() method.
	void wait();
	// Description:
	// This function does not block the thread, but wait until get notify by calling notify() method and returns true.
	auto try_wait() -> bool;
	// Description:
	// Function that notifies to wait() method to unblock the thread, and also
	// it notifies to try_wait() method to unblock as well and return true to proceed.
	void notify();

	// Description:
	// Function that blocks the thread for the given milliseconds.
	auto wait_for(const unsigned long _milliseconds) -> bool;
	// Description:
	// Function that blocks the thread from now to until the given milliseconds.
	auto wait_until(const unsigned long _milliseconds) -> bool;

	// Description:
	// Function that returns the condition variable native handle.
	auto native_handle() -> std::condition_variable::native_handle_type;

protected:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	int m_count;
};

}

#endif // fvkSemaphore_h__