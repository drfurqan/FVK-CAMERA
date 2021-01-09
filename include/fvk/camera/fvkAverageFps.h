#pragma once
#ifndef fvkAverageFps_h__
#define fvkAverageFps_h__

/*********************************************************************************
created:	2013/12/12   01:37AM
modified:	2017/02/09   11:18PM
filename: 	fvkAverageFps.h
file base:	fvkAverageFps
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class to compute the average frames per second.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkClockTime.h"
#include <queue>

namespace R3D
{

class FVK_CAMERA_EXPORT fvkThreadStats
{
public:
	fvkThreadStats() : 
		nfps(0), 
		nframes(0) 
	{ 
	}
	int nfps;		// average frames per second.
	int nframes;	// total number of processed frames.
};

class FVK_CAMERA_EXPORT fvkAverageFps
{

public:
	// Description:
	// Default constructor that takes a number which is the total
	// number to frames to be used for the average calculation.
	explicit fvkAverageFps(const int avg_size = 16);

	// Description:
	// Function that capture the elapsed time between each frame and
	// calculate the average frames per second.
	void update();

	// Description:
	// Function to set number of frames to be used for the average calculation.
	// This should not be called when a thread is executed.
	// Call it before executing the thread.
	void setAverageSize(const int avg_size) { m_avgsize = avg_size; }
	// Description:
	// Function to get number of frames to be used for the average calculation.
	// This should not be called when a thread is executed.
	// Call it before executing the thread.
	auto getAverageSize() const { return m_avgsize; }

	// Description:
	// Function that gives you the average fps and the total number of processed frames.
	auto& getStats() { return stats; }

private:
	std::queue<int> m_fps;
	fvkClockTime m_time;
	fvkThreadStats stats;
	int m_avgsize;
	int m_capture_time;
	int m_fpssum;
	int m_fpssamples;
};

}

#endif // fvkAverageFps_h__