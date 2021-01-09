#pragma once
#ifndef fvkCameraList_h__
#define fvkCameraList_h__

/*********************************************************************************
created:	2017/07/31   03:32PM
filename: 	fvkCameraList.h
file base:	fvkCameraList
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class that gives a list of camera devices with add, remove, and
find features.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkCameraExport.h"

#include <opencv2/opencv.hpp>

#include <vector>
#include <algorithm>

namespace R3D
{

template <typename CAMERA>
class FVK_CAMERA_EXPORT fvkCameraList
{

public:
	// Description:
	// Default constructor to create a list of camera objects.
	fvkCameraList()
	{
	}
	// Description:
	// Default destructor calls clear() which stops all the running threads and releases all the camera devices.
	virtual ~fvkCameraList()
	{
		clear();
	}
	// Description:
	// Function to remove all camera devices from the list.
	// It stops all the running threads and releases all the camera devices.
	void clear()
	{
		for (auto& cam : m_list)
		{
			if (cam)	
				delete cam;
			cam = nullptr;
		}
		m_list.clear();
	}
	// Description:
	// Function to add a unique camera to the list. 
	// If there is a camera with the same index already in the list, that will be not be added to the list.
	auto addUnique(CAMERA* cam)
	{
		auto it = std::find_if(m_list.begin(), m_list.end(),
			[&](const CAMERA* _c)
		{
			return _c->getDeviceIndex() == cam->getDeviceIndex();
		});
		if (it == m_list.end())
		{
			m_list.push_back(cam);
			return true;
		}

		return false;
	}

	// Description:
	// Default constructor that creates a camera object for continuous streaming.
	// _device_index is the index of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// _frame_size is the desired width and height of camera frame.
	// Specifying Size(-1, -1) will do the auto-selection for the captured frame size,
	// normally it enables the 640x480 resolution on most of web cams.
	auto add(const int device_index = 0, const cv::Size& frame_size = cv::Size(-1, -1), const int api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY))
	{
		auto p = new CAMERA(device_index, frame_size, api);
		if (!addUnique(p))
		{
			delete p;
			p = nullptr;
		}

		return p;
	}
	// Description:
	// Default constructor that creates a camera object for continuous streaming.
	// _video_file is the location of the video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// _frame_size is the desired camera frame width and height.
	// Specifying Size(-1, -1) will do the auto-selection for the frame's width and height.
	auto add(const std::string& video_file, const cv::Size& frame_size = cv::Size(-1, -1), const int api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY))
	{
		auto p = new CAMERA(video_file, frame_size, api);
		if (!addUnique(p))
		{
			delete p;
			p = nullptr;
		}

		return p;
	}
	// Description:
	// Constructor that creates a camera object by specifying the camera thread.
	// This constructor is created for the derived classes of fvkCameraThread.
	template <typename CameraThread>
	auto add(CameraThread* ct)
	{
		auto p = new CAMERA(ct);
		if (!addUnique(p))
		{
			delete p;
			p = nullptr;
		}

		return p;
	}
	// Description:
	// Constructor to create a camera object by specifying the camera and processing threads.
	// This constructor is created for the derived classes of both fvkCameraThread and fvkCameraProcessingThread.
	template <typename CameraThread, typename ProcessingThread>
	auto add(CameraThread* ct, ProcessingThread* pt)
	{
		auto p = new CAMERA(ct, pt);
		if (!addUnique(p))
		{
			delete p;
			p = nullptr;
		}

		return p;
	}
	// Description:
	// Function to add a new camera to the list. 
	// It returns NULL if a camera with the same device id is already in the list.
	// Function to add a camera object by specifying the camera and processing threads.
	// This function is created for the derived classes of both fvkCameraThread and fvkCameraProcessingThread.
	auto add(CAMERA* p)
	{
		return addUnique(p);
	}

	// Description:
	// Function to get a pointer to camera by index of the camera list.
	auto get(const  std::size_t index) const
	{
		if (index < 0 || index >= m_list.size())
			return static_cast<CAMERA*>(nullptr);

		return m_list[index];
	}
	// Description:
	// Function to get a pointer to camera device by it's id.
	auto getByIndex(const int device_index) const
	{
		auto it = std::find_if(m_list.begin(), m_list.end(),
			[&device_index](const CAMERA* c)
		{
			return c->getDeviceIndex() == device_index;
		});
		if (it != m_list.end())
			return (*it);

		return static_cast<CAMERA*>(nullptr);
	}

	// Description:
	// Function to remove a camera from the list.
	// It returns true on success.
	auto remove(CAMERA* p)
	{
		if (!p) 
			return false;

		m_list.erase(std::remove(m_list.begin(), m_list.end(), p), m_list.end());
		delete p;
		p = nullptr;
		return true;
	}
	// Description:
	// Function to remove a camera from the list.
	// It returns true on success.
	auto remove(const int device_index)
	{
		return remove(getByIndex(device_index));
	}

	// Description:
	// Function to get the total number of cameras in the list.
	auto getSize() const { return m_list.size(); }
	// Description:
	// Function to a reference to this list.
	auto& getList() { return m_list; }

private:
	std::vector<CAMERA*> m_list;
};

}

#endif // fvkCameraList_h__