#pragma once
#ifndef fvkExport_h__
#define fvkExport_h__

/*********************************************************************************
created:	2017/03/20   02:12PM
filename: 	fvkExport.h
file base:	fvkExport
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Export support for DLL and LIB files.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#  if defined(FVK_DLL)
#    ifdef FVK_LIBRARY
#      define FVK_EXPORT	__declspec(dllexport)
#    else
#      define FVK_EXPORT	__declspec(dllimport)
#    endif /* FVK_LIBRARY */
#  else
#    define FVK_EXPORT
#  endif /* FVK_DLL */


#endif // fvkExport_h__