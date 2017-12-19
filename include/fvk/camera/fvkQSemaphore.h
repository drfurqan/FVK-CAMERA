#pragma once
#ifndef fvkQSemaphore_h__
#define fvkQSemaphore_h__

/*********************************************************************************
created:	2017/02/09   01:19AM
filename: 	fvkQSemaphore.h
file base:	fvkQSemaphore
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

class FVK_EXPORT fvkQSemaphore
{
public:
	// Description:
	// Construct a counting semaphore with an initial value.
	explicit fvkQSemaphore(int _n = 0);
	~fvkQSemaphore();

	// Description:
	// acquire a semaphore count
	// It's actually a "wait" method.
	// It blocks the thread, until get notify by calling release() method.
	void acquire(int _n = 1);
	// Description:
	// try to acquire a semaphore count.
	// It's actually a "try_wait" method.
	// It does not block the thread, but wait until get notify by calling release() method and returns true.
	auto tryAcquire(int _n = 1) -> bool;
	// Description:
	// release one semaphore count.
	// It's actually a "notify" method.
	// It notifies to acquire() method to unblock the thread, and also
	// it notifies to tryAcquire() method to unblock as well as return true to proceed.
	void release(int _n = 1);

	// Description:
	// Function that return total number of available count.
	auto count() -> int;

private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	int m_count;
};

}

#endif // fvkQSemaphore_h__