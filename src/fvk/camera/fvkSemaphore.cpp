/*********************************************************************************
created:	2017/02/09   01:37AM
filename: 	fvkSemaphore.cpp
file base:	fvkSemaphore
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	basic semaphore like QSemaphore functionalities.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkSemaphore.h>

using namespace R3D;

fvkSemaphore::fvkSemaphore(const int _count) : m_count{ _count }
{
}

void fvkSemaphore::notify()
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	++m_count;
	m_cv.notify_one();
}

void fvkSemaphore::wait()
{
	std::unique_lock<std::mutex> lock{ m_mutex };
	m_cv.wait(lock, [&]{ return m_count > 0; });
	--m_count;
}

auto fvkSemaphore::try_wait() -> bool
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	if (m_count > 0)
	{
		--m_count;
		return true;
	}
	return false;
}