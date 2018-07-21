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
	auto addUnique(CAMERA* _cam)
	{
		auto it = std::find_if(m_list.begin(), m_list.end(),
			[&](const CAMERA* _c)
		{
			return _c->getDeviceIndex() == _cam->getDeviceIndex();
		});
		if (it == m_list.end())
		{
			m_list.push_back(_cam);
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
	auto add(const int _device_index = 0, const cv::Size& _frame_size = cv::Size(-1, -1), const int _api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY))
	{
		auto p = new CAMERA(_device_index, _frame_size, _api);
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
	auto add(const std::string& _video_file, const cv::Size& _frame_size = cv::Size(-1, -1), const int _api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY))
	{
		auto p = new CAMERA(_video_file, _frame_size, _api);
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
	auto add(CameraThread* _ct)
	{
		auto p = new CAMERA(_ct);
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
	auto add(CameraThread* _ct, ProcessingThread* _pt)
	{
		auto p = new CAMERA(_ct, _pt);
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
	auto add(CAMERA* _p)
	{
		return addUnique(_p);
	}

	// Description:
	// Function to get a pointer to camera by index of the camera list.
	auto get(const  std::size_t _index) const
	{
		if (_index < 0 || _index >= m_list.size())
			return static_cast<CAMERA*>(nullptr);

		return m_list[_index];
	}
	// Description:
	// Function to get a pointer to camera device by it's id.
	auto getBy(const int _device_index) const
	{
		auto it = std::find_if(m_list.begin(), m_list.end(),
			[&_device_index](const CAMERA* _c)
		{
			return _c->getDeviceIndex() == _device_index;
		});
		if (it != m_list.end())
			return (*it);

		return static_cast<CAMERA*>(nullptr);
	}

	// Description:
	// Function to remove a camera from the list.
	// It returns true on success.
	auto remove(CAMERA* _p)
	{
		if (!_p) 
			return false;

		m_list.erase(std::remove(m_list.begin(), m_list.end(), _p), m_list.end());
		delete _p;
		_p = nullptr;
		return true;
	}
	// Description:
	// Function to remove a camera from the list.
	// It returns true on success.
	auto remove(const int _device_index)
	{
		return remove(getBy(_device_index));
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