/*********************************************************************************
created:	2016/11/06   01:54AM
modified:	2017/02/09   11:26PM
filename: 	fvkSemaphoreBuffer.h
file base:	fvkSemaphoreBuffer
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	creating a thread-safe synchronized queue and to achieve
semaphore type functionality.
Semaphores in C++
The C++ standard does not define a semaphore type. You can write your own
with an atomic counter, a mutex and a condition variable if you need, but
most uses of semaphores are better replaced with mutexes and/or condition
variables anyway.
Unfortunately, for those cases where semaphores really are what you want,
using a mutex and a condition variable adds overhead, and there is nothing
in the C++ standard to help.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkSemaphoreBuffer.h>

using namespace R3D;