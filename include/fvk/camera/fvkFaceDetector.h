#pragma once
#ifndef fvkFaceDetector_h__
#define fvkFaceDetector_h__

/*********************************************************************************
created:	2017/02/10   02:38AM
filename: 	fvkFaceDetector.h
file base:	fvkFaceDetector
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Class to detect face in the specified frame.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkCameraExport.h"

#include <opencv2/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <string>

namespace R3D
{

class FVK_CAMERA_EXPORT fvkFaceDetector
{
public:
	// Description:
	// Default constructor to initiate the object.
	fvkFaceDetector();
	// Description:
	// Default destructor that deletes the cascade.
	~fvkFaceDetector();

	// Description:
	// Function to reset the default parameters.
	void reset();

	// Description:
	// Function to load a classifier from a file.
	auto setFaceCascade(const std::string& cascade_file_path) -> bool;
	// Description:
	// Function to get a pointer to face cascade.
	auto getFaceCascade() const { return m_face_cascade; }
	// Description:
	// Function to get a rectangle around the detected face.
	auto getRect() const -> cv::Rect;
	// Description:
	// Function to get a position of the detected face.
	auto getPosition() const -> cv::Point;
	// Description:
	// Function to set the width of the detectable face.
	void setResizedWidth(const int width);
	// Description:
	// Function to get the width of the detectable face.
	auto getResizedWidth() const -> int;
	// Description:
	// Function to set the maximum duration for the template matching.
	void setTemplateMatchingMaxDuration(const double s);
	// Description:
	// Function to get the maximum duration for the template matching.
	auto templateMatchingMaxDuration() const -> double;
	// Description:
	// Function that detects all faces in the given frame and get the biggest face.
	void detectFaceAllSizes(const cv::Mat& frame);
	// Description:
	// Function that detects the biggest face in the given frame and track it.
	auto detect(cv::Mat& frame) -> cv::Point;
	// Description:
	// Overloaded operator of the above function.
	auto operator >> (cv::Mat& frame) -> cv::Point;

private:
	static auto doubleRectSize(const cv::Rect& input_rect, const cv::Rect& frame_size) -> cv::Rect;
	static auto biggestFace(std::vector<cv::Rect>& faces) -> cv::Rect;
	static auto centerOfRect(const cv::Rect& rect) -> cv::Point;
	static auto getFaceTemplate(const cv::Mat& frame, cv::Rect face) -> cv::Mat;
	void detectFaceAroundRoi(const cv::Mat& frame);
	void detectFacesTemplateMatching(const cv::Mat& frame);

	static const double TICK_FREQUENCY;

	cv::CascadeClassifier* m_face_cascade;
	std::vector<cv::Rect> m_all_faces;
	cv::Rect m_tracked_face;
	cv::Rect m_face_roi;
	cv::Mat m_face_template;
	cv::Mat m_matching_result;
	bool m_template_matching_running;
	int64 m_template_matching_start_time;
	int64 m_template_matching_current_time;
	bool m_found_face;
	double m_scale;
	int m_resized_width;
	cv::Point m_face_position;
	double m_template_matching_max_duration;
};

class FVK_CAMERA_EXPORT fvkSimpleFaceDetector
{
public:
	// Description:
	// Default constructor to initialize a face detector.
	fvkSimpleFaceDetector();
	// Description:
	// Default destructor to destroy the data.
	virtual ~fvkSimpleFaceDetector() = default;

	// Description:
	// Function to load a classifier from a file.
	// filename Name of the file from which the classifier is loaded.
	// It returns true on success.
	auto loadCascadeClassifier(const std::string& filename) -> bool;
	// Description:
	// Function to get the file path of the classifier.
	auto getCascadeClassifierFilePath() const { return m_filepath; }

	// Description:
	// Function to get a reference to face detector.
	auto& get() { return m_fd; }

	// Description:
	// Function that detect all the faces in the given frame.
	// It selects the biggest face for tracking.
	virtual void detect(cv::Mat& frame, const int frame_delay_in_detection = 5);

protected:
	std::string m_filepath;
	fvkFaceDetector m_fd;
	int m_nframes;
};

}

#endif // fvkFaceDetector_h__