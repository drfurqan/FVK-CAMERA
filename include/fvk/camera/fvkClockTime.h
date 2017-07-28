#pragma once
#ifndef fvkTime_h__
#define fvkTime_h__

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
	// Function that returns the elapsed time and restart the clock.
	double restart();
	// Description:
	// Function to stop the clock and print the execution time (if print = true). 
	// It returns the elapsed time between start() and stop().
	double stop(bool _print = false);
	// Description:
	// Function that returns the elapsed time (in seconds) between start() and stop().
	double elapsed() const { return m_elapstime; }
	// Description:
	// Function to print the elapsed time. 
	void print();
	// Description:
	// Function to get the current time.
	static double getCurrentTime();

private:
	std::chrono::time_point<std::chrono::system_clock> m_startime;
	std::string m_label;
	double m_elapstime;
};

}

#endif // fvkTime_h__
