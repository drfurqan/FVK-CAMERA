/*********************************************************************************
created:	2015/04/21   03:37AM
modified:	2017/02/09   11:37PM
filename: 	fvkClockTime.cpp
file base:	fvkClockTime
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Class for elapsed time using std::chrono.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkClockTime.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace R3D;

fvkClockTime::fvkClockTime(bool _start_time, const std::string& _label) : 
m_label(_label)
{
	if (_start_time) 
		start();
}
void fvkClockTime::print()
{ 
#ifdef _DEBUG
	std::cout << m_label << " took " << m_elapstime << " milliseconds." << std::endl;
#endif // _DEBUG
}
auto fvkClockTime::getCurrentTime() -> int
{ 
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
}
void fvkClockTime::start() 
{ 
	m_startime = std::chrono::system_clock::now();
}
auto fvkClockTime::restart() -> int
{
	auto t = stop(false);
	start();
	return t;
}
auto fvkClockTime::stop(bool _print) -> int
{
	auto t = std::chrono::system_clock::now() - m_startime;
	m_elapstime = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(t).count());
#ifdef _DEBUG
	if (_print)
		std::cout << m_label << " took " << m_elapstime << " milliseconds." << std::endl;
#endif // _DEBUG
	return m_elapstime;
}

auto fvkClockTime::getLocalTime(const char* _format) -> std::string
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), _format);
	return ss.str();
}