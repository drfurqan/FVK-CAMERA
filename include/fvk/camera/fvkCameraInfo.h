#pragma once
#ifndef fvkCameraInfo_h__
#define fvkCameraInfo_h__

/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2018/05/18   3:35AM
filename: 	fvkCameraInfo.h
file base:	fvkCameraInfo
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	useful functions for video devices.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkExport.h"

#include <string>
#include <vector>

namespace R3D
{

class FVK_EXPORT fvkCameraInfo
{

public:
	// Description:
	// Functions that finds the available devices and
	// returns all found devices with their Ids and Names.
	static std::vector< std::pair<int, std::string> > availableCameras();
};

}

#endif // fvkCameraInfo_h__