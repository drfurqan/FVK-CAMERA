/*********************************************************************************
created:	2013/12/12   01:37AM
modified:	2017/02/09   11:18PM
filename: 	fvkAverageFps.cpp
file base:	fvkAverageFps
file ext:	cpp
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

#include <fvk/camera/fvkAverageFps.h>
#include <iostream>
using namespace R3D;

fvkAverageFps::fvkAverageFps(int _avg_size) :
m_avgsize(_avg_size),
m_capture_time(0),
m_fpssum(0),
m_fpssamples(0)
{
}
void fvkAverageFps::update()
{
	m_capture_time = m_time.restart();		// captured time in which one frame has been processed.

	if (m_capture_time > 0)
	{
		m_fps.push(static_cast<int>(1000 / m_capture_time));
		m_fpssamples++;
	}

	if (m_fps.size() > static_cast<std::size_t>(m_avgsize))
		m_fps.pop();

	if ((m_fps.size() == static_cast<std::size_t>(m_avgsize)) && (m_fpssamples == m_avgsize))
	{
		while (!m_fps.empty())
		{
			m_fpssum += m_fps.front();				// calculate the sum and clear the queue.
			m_fps.pop();
		}

		stats.fps = static_cast<int>(m_fpssum / m_avgsize);
		m_fpssum = 0;
		m_fpssamples = 0;
	}

	if (stats.nframes < 2147483647)	// INT_MAX
		stats.nframes++;
}
