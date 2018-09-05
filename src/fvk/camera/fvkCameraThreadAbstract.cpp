/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2017/02/09   11:18PM
filename: 	fvkCameraThreadAbstract.cpp
file base:	fvkCameraThreadAbstract
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	OpenCV camera class interface.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkCameraThreadAbstract.h>

using namespace R3D;

fvkCameraThreadAbstract::fvkCameraThreadAbstract(const int _device_index, const cv::Size& _frame_size) :
m_device_index(_device_index),
m_frame_size(_frame_size)
{
}

fvkCameraThreadAbstract::~fvkCameraThreadAbstract()
{
}

auto fvkCameraThreadAbstract::open() -> bool
{
	return open(m_device_index);
}