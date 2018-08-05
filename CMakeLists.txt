# ------------------------------------------------------------------------------
# About cmake
# ------------------------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.11)
# set(CMAKE_DEBUG_POSTFIX d)

# ------------------------------------------------------------------------------
# Library name
# ------------------------------------------------------------------------------

project (fvk_camera)

# file(READ VERSION FVK_CAMERA_VERSION)

set(VERSION_MAJOR 1)
set(VERSION_MINOR_0 1)
set(VERSION_MINOR_1 4)
set(VERSION_PLAIN ${VERSION_MAJOR}.${VERSION_MINOR_0}.${VERSION_MINOR_1})

# ------------------------------------------------------------------------------
# Library's source files (*.cpp files)
# ------------------------------------------------------------------------------

option(OPTION_BUILD_EXAMPLES "Build examples written in FVK_CAMERA" ON)

if(OPTION_BUILD_EXAMPLES)
   add_subdirectory(examples)
endif(OPTION_BUILD_EXAMPLES)

# ------------------------------------------------------------------------------
# Library's source files (*.cpp files)
# ------------------------------------------------------------------------------

set(CPPFILES
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkAverageFps.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkCamera.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkImageProcessing.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkProcessingThread.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkCameraThread.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkCameraThreadAbstract.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkCameraThreadOpenCV.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkClockTime.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkFaceDetector.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkImagePlot.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkQSemaphore.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkSemaphore.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkSemaphoreBuffer.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkThread.cpp
${CMAKE_SOURCE_DIR}/src/fvk/camera/fvkVideoWriter.cpp
)

# ------------------------------------------------------------------------------
# Library's header files (*.h files)
# ------------------------------------------------------------------------------

set(HEADERFILES
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkAverageFps.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkCamera.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkCameraList.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkImageProcessing.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkProcessingThread.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkCameraThread.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkCameraThreadAbstract.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkCameraThreadOpenCV.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkClockTime.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkCameraExport.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkFaceDetector.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkImagePlot.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkQSemaphore.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkSemaphore.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkSemaphoreBuffer.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkThread.h
${CMAKE_SOURCE_DIR}/include/fvk/camera/fvkVideoWriter.h
)

# ------------------------------------------------------------------------------
# Prepare source files for shared and static FVK library
# ------------------------------------------------------------------------------

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

set (SHARED_FILES ${CPPFILES})

set (STATIC_FILES ${SHARED_FILES})

# add_definitions(-DFVK_EXPORT)

# ------------------------------------------------------------------------------
# Add source and header files and define the statis or shared version
# ------------------------------------------------------------------------------

if (WIN32)
	add_library (${PROJECT_NAME} STATIC ${STATIC_FILES} ${HEADERFILES})
else()
	add_library (${PROJECT_NAME} SHARED ${STATIC_FILES} ${HEADERFILES})
endif()

# ------------------------------------------------------------------------------
# Required libraries for this library which is FLE
# ------------------------------------------------------------------------------

if (WIN32)
	find_package(OpenCV REQUIRED opencv_world)
else()
	find_package(OpenCV REQUIRED)
endif()

# ------------------------------------------------------------------------------
# Link all the required and optional libraries to this library
# ------------------------------------------------------------------------------

set (OPTIONAL_LIBS)

if (WIN32)
    list(APPEND OPTIONAL_LIBS comctl32)
    list(APPEND OPTIONAL_LIBS ws2_32)
    list(APPEND OPTIONAL_LIBS vfw32)
endif (WIN32)

target_link_libraries(
${PROJECT_NAME}
${OPTIONAL_LIBS} 
${OpenCV_LIBS}
)

# ------------------------------------------------------------------------------
# Add paths to OpenCV include directories
# ------------------------------------------------------------------------------

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})

# ------------------------------------------------------------------------------
# Add proprocessor defininations
# ------------------------------------------------------------------------------

if (WIN32)
    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEF:ExportedFunctions.def /NODEFAULTLIB")
	set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIBS:LIBCMT")
	set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIBS:MSVCRT")
	set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIBS:MSVCPRTD")
    endif()
endif()

# ------------------------------------------------------------------------------
# set output name of the library with major and minor version
# ------------------------------------------------------------------------------

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION_PLAIN})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${VERSION_PLAIN})

if (WIN32)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_${VERSION_MAJOR}${VERSION_MINOR_0}${VERSION_MINOR_1})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_OUTPUT_NAME ${PROJECT_NAME}_${VERSION_MAJOR}${VERSION_MINOR_0}${VERSION_MINOR_1}d)
endif()

# ------------------------------------------------------------------------------
# installation
# ------------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} 
RUNTIME DESTINATION bin           
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib)

install(DIRECTORY 
${PROJECT_SOURCE_DIR}/include/fvk/camera
DESTINATION include/fvk 
USE_SOURCE_PERMISSIONS
PATTERN ".svn" EXCLUDE
)