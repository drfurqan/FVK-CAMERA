#pragma once
#ifndef fvkClockTime_h__
#define fvkClockTime_h__

/*********************************************************************************
created:	2015/04/21   03:37AM
modified:	2017/02/09   11:37PM
filename: 	fvkClockTime.h
file base:	fvkClockTime
file ext:	h
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

#include "fvkExport.h"

#include <chrono>
#include <string>

namespace R3D
{

class FVK_EXPORT fvkClockTime
{
public:
	// Usage Example:
	// fvkClockTime t(true);
	// dosomething();
	// t.stop(true);

	// Default constructor.
	// Set _start_time to true if clock needs to be started with the constructor.
	// _label is the label of this class object.
	fvkClockTime(bool _start_time = true, const std::string& _label = std::string("Process "));

	// Description:
	// Function to set a label to this class object.
	void setLabel(const std::string& _label) { m_label = _label; }
	// Description:
	// Function to get the label of this class object.
	std::string getLabel() const { return m_label; }
	// Description:
	// Function to start the clock.
	void start();
	// Description:
	// Function that returns the elapsed time (in milliseconds) and restart the clock.
	int restart();
	// Description:
	// Function to stop the clock and print the execution time (if print = true). 
	// It returns the elapsed time (in milliseconds) between start() and stop().
	int stop(bool _print = false);
	// Description:
	// Function that returns the elapsed time (in milliseconds) between start() and stop().
	int elapsed() const { return m_elapstime; }
	// Description:
	// Function to print the elapsed time (in milliseconds). 
	void print();

	// Description:
	// Function to get the current time in milliseconds.
	static int getCurrentTime();
	// Description:
	// Function to get the local date and time.
	static std::string getLocalTime(const char* _format = "%Y-%m-%d %X");

private:
	std::chrono::time_point<std::chrono::system_clock> m_startime;
	std::string m_label;
	int m_elapstime;
};

}

#endif // fvkClockTime_h__