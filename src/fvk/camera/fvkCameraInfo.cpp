/*********************************************************************************
created:	2016/04/30   01:37AM
modified:	2018/05/18   3:35AM
filename: 	fvkCameraInfo.cpp
file base:	fvkCameraInfo
file ext:	cpp
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

#include <fvk/camera/fvkCameraInfo.h>

#ifdef _WIN32
#include <Dshow.h>
#endif // _WIN32

#include <iostream>

using namespace R3D;

static char m_device_names[20][255];

// Description:
// Functions that finds the available devices and
// returns the total number of found devices.
int findDevices(const bool silent) 
{
	auto deviceCounter = 0;
#ifdef _WIN32
	ICreateDevEnum *pDevEnum = nullptr;
	IEnumMoniker *pEnum = nullptr;

	auto hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, reinterpret_cast<void**>(&pDevEnum));
	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		if (hr == S_OK) 
		{
			if (!silent) std::cout << "Looking For Capture Devices...\n";

			IMoniker *pMoniker = nullptr;
			while (pEnum->Next(1, &pMoniker, nullptr) == S_OK) 
			{
				IPropertyBag *pPropBag;
				hr = pMoniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, reinterpret_cast<void**>(&pPropBag));
				if (FAILED(hr)) 
				{
					pMoniker->Release();
					continue;  // Skip this one, maybe the next one will work.
				}

				// Find the description or friendly name.
				VARIANT var_name;
				VariantInit(&var_name);
				hr = pPropBag->Read(L"Description", &var_name, nullptr);
				if (FAILED(hr)) 
					hr = pPropBag->Read(L"FriendlyName", &var_name, nullptr);

				if (SUCCEEDED(hr))
				{
					pPropBag->Read(L"FriendlyName", &var_name, nullptr);

					auto count = 0;
					const int maxLen = sizeof(m_device_names[0]) / sizeof(m_device_names[0][0]) - 2;
					while (var_name.bstrVal[count] != 0x00 && count < maxLen) 
					{
						m_device_names[deviceCounter][count] = static_cast<char>(var_name.bstrVal[count]);
						count++;
					}
					m_device_names[deviceCounter][count] = 0;

					if (!silent) std::cout << "Device: Id [" << deviceCounter << "] Name [" << m_device_names[deviceCounter] << "]\n";
				}

				pPropBag->Release();
				pPropBag = nullptr;

				pMoniker->Release();
				pMoniker = nullptr;

				deviceCounter++;
			}

			pDevEnum->Release();
			pDevEnum = nullptr;

			pEnum->Release();
			pEnum = nullptr;
		}

		if (!silent) std::cout << "Device(s) found\n\n" << deviceCounter << "\n";
	}
#endif // _WIN32

	return deviceCounter;
}

// Description:
// Function that returns the name of the given device id.
// This function needs to be called after findDevices();
std::string getDeviceName(const int device_id)
{
	if (device_id < 0 || device_id >= 20)
		return "";
	return m_device_names[device_id];
}

std::vector< std::pair<int, std::string> > fvkCameraInfo::availableCameras()
{
	std::vector< std::pair<int, std::string> > devices;
	const auto n = findDevices(false);
	if (n == 0)
		return devices;

	devices.reserve(n);
	for (auto i = 0; i < n; i++)
		devices.push_back(std::make_pair(i, getDeviceName(i)));

	return devices;
}