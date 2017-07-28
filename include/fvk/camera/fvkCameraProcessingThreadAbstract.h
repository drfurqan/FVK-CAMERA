#pragma once
#ifndef fvkCameraProcessingThreadAbstract_h__
#define fvkCameraProcessingThreadAbstract_h__

/*********************************************************************************
created:	2013/12/20   01:37AM
modified:	2017/02/09   11:43PM
filename: 	fvkCameraProcessingThreadAbstract.h
file base:	fvkCameraProcessingThreadAbstract
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	OpenCV camera processing thread interface.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkExport.h"

#include "opencv2/opencv.hpp"

namespace R3D
{

class FVK_EXPORT fvkCameraProcessingThreadAbstract
{

public:
	// Description:
	// Default constructor.
	virtual ~fvkCameraProcessingThreadAbstract() {}

	// Description:
	// Pure virtual function that starts the thread.
	virtual void run() = 0;

	// Description:
	// Pure virtual function to be overridden to stop the processing and it's thread.
	virtual void stop() = 0;

	// Description:
	// Pure virtual function to get the current grabbed frame.
	virtual cv::Mat getFrame() = 0;

	// Description:
	// Pure virtual function that trigger the saveFrameToDisk function once.
	virtual void saveFrameOnClick() = 0;
};

}

#endif // fvkCameraProcessingThreadAbstract_h__
