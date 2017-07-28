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

purpose:	Class to compute the average frames per second.

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

class FVK_EXPORT fvkAverageFpsStats
{
public:
	fvkAverageFpsStats() : fps(0), nframes(0) { }
	int fps, nframes;
};

class FVK_EXPORT fvkAverageFps
{

public:
	// Description:
	// Default constructor that takes a number which is the total
	// number to frames to be used for the average calculation.
	fvkAverageFps(int _avg_size = 16);

	// Description:
	// Function that capture the elapsed time between each frame and
	// calculate the average frames per second.
	void update();

	// Description:
	// Function to set number of frames to be used for the average calculation.
	void setAverageSize(int _avg_size) { m_avgsize = _avg_size; }
	// Description:
	// Function to get number of frames to be used for the average calculation.
	int getAverageSize() const { return m_avgsize; }

	// Description:
	// public stat member that gives you the average fps and the total number
	// of processed frames.
	fvkAverageFpsStats stats;

private:
	std::queue<double> m_fps;
	fvkClockTime m_time;
	int m_avgsize;
	double m_capture_time;
	double m_fpssum;
	int m_fpssamples;
};

}

#endif // fvkAverageFps_h__