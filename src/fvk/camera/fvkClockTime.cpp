/*********************************************************************************
created:	2015/04/21   03:37AM
modified:	2017/02/10   11:37PM
filename: 	fvkTime.cpp
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
	std::cout << m_label << " took " << m_elapstime << " sec." << std::endl;
#endif // _DEBUG
}
double fvkClockTime::getCurrentTime() 
{ 
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(now).count());
}
void fvkClockTime::start() 
{ 
	m_startime = std::chrono::system_clock::now();
}
double fvkClockTime::restart()
{
	double t = stop(false);
	start();
	return t;
}
double fvkClockTime::stop(bool _print)
{
	std::chrono::duration<double> elapsed_sec = std::chrono::system_clock::now() - m_startime;
	m_elapstime = elapsed_sec.count();
	if (_print)
	{
#ifdef _DEBUG
		std::cout << m_label << " took " << m_elapstime << " sec." << std::endl;
#endif // _DEBUG
	}
	return m_elapstime;
}