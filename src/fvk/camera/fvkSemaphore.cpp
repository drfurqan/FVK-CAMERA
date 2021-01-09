/*********************************************************************************
created:	2017/02/09   01:37AM
filename: 	fvkSemaphore.cpp
file base:	fvkSemaphore
file ext:	cpp
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

#include <fvk/camera/fvkSemaphore.h>

using namespace R3D;

fvkSemaphore::fvkSemaphore(const int count) : m_count{ count }
{
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

void fvkSemaphore::notify()
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	++m_count;
	m_cv.notify_one();
}

auto fvkSemaphore::wait_for(const unsigned long milliseconds) -> bool
{
	std::unique_lock<std::mutex> lock{ m_mutex };
	auto finished = m_cv.wait_for(lock, std::chrono::milliseconds(milliseconds), [&] { return m_count > 0; });
	if (finished)
		--m_count;
	return finished;
}

auto fvkSemaphore::wait_until(const unsigned long milliseconds) -> bool
{
	std::unique_lock<std::mutex> lock{ m_mutex };
	auto finished = m_cv.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(milliseconds), [&] { return m_count > 0; });
	if (finished)
		--m_count;
	return finished;
}

auto fvkSemaphore::native_handle() ->std::condition_variable::native_handle_type
{
	return m_cv.native_handle();
}