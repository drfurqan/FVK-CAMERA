## Fast Visualization Kit (FVK)

The Fast Visualization Kit (FVK) is a software package for 3D computer graphics, image processing, and visualization. It consists of a C++ class library with several 3D geometric as well as vision algorithms. It provides C++ classes for 2D-to-3D conversion algorithms specifically for DFP based structured light technology, various algorithm for mesh and pointcloud processig, and a complete software interface for developing fast 3D graphics and vision based applications. 
FVK supports visualization algorithms including scalar, vector, texture, and volumetric methods, as well as advanced modeling techniques such as implicit modeling, polygon reduction, mesh smoothing, cutting, contouring, and Delaunay triangulation.

The FVK-CAMERA is one of its modules that gives multithreading based camera handlings using OpenCV. It can be utilized with any GUI based library to handle multiple cameras with multithreaded interface.

## Repository structure

FVK-CAMERA Library is implemented in C++ (but provides both a C and a 
C++ish interface). The directory structure is: <br/>

-  /examples (Examples to illustrate possible use cases for FVK-CAMERA)
-  /include/fvk/camera (Public header C and C++ header files) <br/>	
-  /lib (Weblinks for Static libraries for VS2010, VS2013, VS2015) <br/>	
-  /src/fvk/camera	 (Source code) <br/>						
<br/>

## Building
Take a look into the `INSTALL` file. The build system is CMake, if you already used CMake before there is a good chance you know what to do. <br/>	

Video tutorial that explains the build procedure using CMake (three libraries OpenCV, FLTK, and FLE are compiled in this video): <br/>	

[![How to Build OpenCV, FLTK, FL-Essentials using CMake](how_to_build.png)](https://youtu.be/bwVpf_uKaGo "How to Build OpenCV, FLTK, FL-Essentials using CMake")

## Required Libraries
-  OpenCV >= 2.x.x

## Author
Written by  Dr. Furqan Ullah.

## Reporting Bugs
Report bugs to drfurqan@users.noreply.github.com.

## Copyright
Copyright (c) 2017-2018 Dr. Furqan Ullah.

## See Also
Source code at: <https://github.com/drfurqan/FL-Essentials>.