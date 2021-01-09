/*********************************************************************************
created:	2017/02/09   01:19AM
filename: 	fvkQSemaphore.cpp
file base:	fvkQSemaphore
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

#include <fvk/camera/fvkQSemaphore.h>

using namespace R3D;

fvkQSemaphore::fvkQSemaphore(const int n /*= 0*/) : m_count(n)
{

}

fvkQSemaphore::~fvkQSemaphore()
{

}

void fvkQSemaphore::release(int n /*= 1*/)
{
	std::lock_guard<std::mutex> lk(m_mutex);
	m_count += n;
	m_cv.notify_all();
}

void fvkQSemaphore::acquire(int n /*= 1*/)
{
	std::unique_lock<std::mutex> lk(m_mutex);
	while (n > m_count)
		m_cv.wait(lk);
	m_count -= n;
}

auto fvkQSemaphore::tryAcquire(int n /*= 1*/) -> bool
{
	std::lock_guard<std::mutex> lk(m_mutex);
	if (n > m_count) return false;
	m_count -= n;
	return true;
}

auto fvkQSemaphore::count() -> int
{
	std::lock_guard<std::mutex> lk(m_mutex);
	return m_count;
}

