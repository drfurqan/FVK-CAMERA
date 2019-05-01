/*********************************************************************************
created:	2017/02/10   02:38AM
filename: 	fvkFaceDetector.cpp
file base:	fvkFaceDetector
file ext:	cpp
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

#include <fvk/camera/fvkFaceDetector.h>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace R3D;

const double fvkFaceDetector::TICK_FREQUENCY = cv::getTickFrequency();

fvkFaceDetector::fvkFaceDetector() : 
m_face_cascade(nullptr),
m_template_matching_running(false),
m_template_matching_start_time(0),
m_template_matching_current_time(0),
m_found_face(false),
m_scale(1),
m_resized_width(320),
m_template_matching_max_duration(3)
{
}
fvkFaceDetector::~fvkFaceDetector()
{
	if (m_face_cascade) 
		delete m_face_cascade;
	m_face_cascade = nullptr;
}
void fvkFaceDetector::reset()
{
	m_template_matching_running = false;
	m_found_face = false;
}

auto fvkFaceDetector::setFaceCascade(const std::string& _cascade_file_path) -> bool
{
	if (_cascade_file_path.empty()) 
		return false;

	if (m_face_cascade == nullptr)
	{
		m_face_cascade = new cv::CascadeClassifier(_cascade_file_path);
	}
	else
	{
		if (!m_face_cascade->load(_cascade_file_path))
		{
			std::cerr << "ERROR: couldn't create cascade classifier. Make sure the file exists." << std::endl;
			return false;
		}
	}

	if (m_face_cascade->empty())
	{
		std::cerr << "ERROR: couldn't create cascade classifier. Make sure the file exists." << std::endl;
		return false;
	}

	reset();

	return true;
}

void fvkFaceDetector::setResizedWidth(const int _width)
{
	m_resized_width = std::max(_width, 1);
}

auto fvkFaceDetector::getResizedWidth() const -> int
{
	return m_resized_width;
}

auto fvkFaceDetector::getRect() const -> cv::Rect
{
	auto face_rect = m_tracked_face;
	face_rect.x = static_cast<int>(face_rect.x / m_scale);
	face_rect.y = static_cast<int>(face_rect.y / m_scale);
	face_rect.width = static_cast<int>(face_rect.width / m_scale);
	face_rect.height = static_cast<int>(face_rect.height / m_scale);
	return face_rect;
}

auto fvkFaceDetector::getPosition() const -> cv::Point
{
	return cv::Point(static_cast<int>(m_face_position.x / m_scale), static_cast<int>(m_face_position.y / m_scale));
}

void fvkFaceDetector::setTemplateMatchingMaxDuration(const double _s)
{
	m_template_matching_max_duration = _s;
}

auto fvkFaceDetector::templateMatchingMaxDuration() const -> double
{
	return m_template_matching_max_duration;
}

auto fvkFaceDetector::doubleRectSize(const cv::Rect& inputRect, const cv::Rect& frameSize) -> cv::Rect
{
	cv::Rect outputRect;
	// Double rect size
	outputRect.width = inputRect.width * 2;
	outputRect.height = inputRect.height * 2;

	// Center rect around original center
	outputRect.x = inputRect.x - inputRect.width / 2;
	outputRect.y = inputRect.y - inputRect.height / 2;

	// Handle edge cases
	if (outputRect.x < frameSize.x) 
	{
		outputRect.width += outputRect.x;
		outputRect.x = frameSize.x;
	}
	if (outputRect.y < frameSize.y) 
	{
		outputRect.height += outputRect.y;
		outputRect.y = frameSize.y;
	}

	if (outputRect.x + outputRect.width > frameSize.width) 
		outputRect.width = frameSize.width - outputRect.x;
	if (outputRect.y + outputRect.height > frameSize.height) 
		outputRect.height = frameSize.height - outputRect.y;

	return outputRect;
}

auto fvkFaceDetector::centerOfRect(const cv::Rect& rect) -> cv::Point
{
	return cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

auto fvkFaceDetector::biggestFace(std::vector<cv::Rect>& faces) -> cv::Rect
{
	assert(!faces.empty());

	auto biggest = &faces[0];
	for (auto &face : faces) 
	{
		if (face.area() < biggest->area())
			biggest = &face;
	}
	return *biggest;
}

/*
* Face template is small patch in the middle of detected face.
*/
auto fvkFaceDetector::getFaceTemplate(const cv::Mat& _frame, cv::Rect _face) -> cv::Mat
{
	_face.x += _face.width / 4;
	_face.y += _face.height / 4;
	_face.width /= 2;
	_face.height /= 2;

	auto face_template = _frame(_face).clone();
	return face_template;
}

void fvkFaceDetector::detectFaceAllSizes(const cv::Mat& _frame)
{
	// Minimum face size is 1/5th of screen height
	// Maximum face size is 2/3rds of screen height
	m_face_cascade->detectMultiScale(
		_frame, m_all_faces, 1.1, 3, 0,
		cv::Size(_frame.rows / 5, _frame.rows / 5),
		cv::Size(_frame.rows * 2 / 3, _frame.rows * 2 / 3));

	if (m_all_faces.empty()) return;

	m_found_face = true;

	// Locate biggest face
	m_tracked_face = biggestFace(m_all_faces);

	// Copy face template
	m_face_template = getFaceTemplate(_frame, m_tracked_face);

	// Calculate roi
	m_face_roi = doubleRectSize(m_tracked_face, cv::Rect(0, 0, _frame.cols, _frame.rows));

	// Update face position
	m_face_position = centerOfRect(m_tracked_face);
}

void fvkFaceDetector::detectFaceAroundRoi(const cv::Mat& _frame)
{
	// Detect faces sized +/-20% off biggest face in previous search
	m_face_cascade->detectMultiScale(
		_frame(m_face_roi), m_all_faces, 1.1, 3, 0,
		cv::Size(m_tracked_face.width * 8 / 10, m_tracked_face.height * 8 / 10),
		cv::Size(m_tracked_face.width * 12 / 10, m_tracked_face.width * 12 / 10));

	if (m_all_faces.empty())
	{
		// Activate template matching if not already started and start timer
		m_template_matching_running = true;
		if (m_template_matching_start_time == 0)
			m_template_matching_start_time = cv::getTickCount();
		return;
	}

	// Turn off template matching if running and reset timer
	m_template_matching_running = false;
	m_template_matching_current_time = m_template_matching_start_time = 0;

	// Get detected face
	m_tracked_face = biggestFace(m_all_faces);

	// Add roi offset to face
	m_tracked_face.x += m_face_roi.x;
	m_tracked_face.y += m_face_roi.y;

	// Get face template
	m_face_template = getFaceTemplate(_frame, m_tracked_face);

	// Calculate roi
	m_face_roi = doubleRectSize(m_tracked_face, cv::Rect(0, 0, _frame.cols, _frame.rows));

	// Update face position
	m_face_position = centerOfRect(m_tracked_face);
}

void fvkFaceDetector::detectFacesTemplateMatching(const cv::Mat& _frame)
{
	// Calculate duration of template matching
	m_template_matching_current_time = cv::getTickCount();
	const auto duration = static_cast<double>(m_template_matching_current_time - m_template_matching_start_time) / TICK_FREQUENCY;

	// If template matching lasts for more than 2 seconds face is possibly lost
	// so disable it and redetect using cascades
	if (duration > m_template_matching_max_duration) 
	{
		m_found_face = false;
		m_template_matching_running = false;
		m_template_matching_start_time = m_template_matching_current_time = 0;
	}

	// Template matching with last known face 
	//cv::matchTemplate(frame(m_faceRoi), m_faceTemplate, m_matchingResult, CV_TM_CCOEFF);
	cv::matchTemplate(_frame(m_face_roi), m_face_template, m_matching_result, cv::TM_SQDIFF_NORMED);
	cv::normalize(m_matching_result, m_matching_result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	double min, max;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(m_matching_result, &min, &max, &min_loc, &max_loc);

	// Add roi offset to face position
	min_loc.x += m_face_roi.x;
	min_loc.y += m_face_roi.y;

	// Get detected face
	//m_trackedFace = cv::Rect(maxLoc.x, maxLoc.y, m_trackedFace.width, m_trackedFace.height);
	m_tracked_face = cv::Rect(min_loc.x, min_loc.y, m_face_template.cols, m_face_template.rows);
	m_tracked_face = doubleRectSize(m_tracked_face, cv::Rect(0, 0, _frame.cols, _frame.rows));

	// Get new face template
	m_face_template = getFaceTemplate(_frame, m_tracked_face);

	// Calculate face roi
	m_face_roi = doubleRectSize(m_tracked_face, cv::Rect(0, 0, _frame.cols, _frame.rows));

	// Update face position
	m_face_position = centerOfRect(m_tracked_face);
}

auto fvkFaceDetector::detect(cv::Mat& _frame) -> cv::Point
{
	// Downscale frame to m_resizedWidth width - keep aspect ratio
	m_scale = static_cast<double>(std::min(m_resized_width, _frame.cols) / static_cast<double>(_frame.cols));
	const auto resized_frame_size = cv::Size(static_cast<int>(m_scale*_frame.cols), static_cast<int>(m_scale*_frame.rows));
	if (resized_frame_size.width > (_frame.cols/2) || resized_frame_size.height > (_frame.rows/2))
		return m_face_position;

	cv::Mat resized_frame;
	cv::resize(_frame, resized_frame, resized_frame_size);

	if (!m_found_face)
	{
		detectFaceAllSizes(resized_frame); // Detect using cascades over whole image
	}
	else 
	{
		detectFaceAroundRoi(resized_frame); // Detect using cascades only in ROI
		if (m_template_matching_running) 
			detectFacesTemplateMatching(resized_frame); // Detect using template matching
	}

	return m_face_position;
}

auto fvkFaceDetector::operator >> (cv::Mat& _frame) -> cv::Point
{
	return detect(_frame);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

fvkSimpleFaceDetector::fvkSimpleFaceDetector() : 
m_filepath(""),
m_nframes(0)
{
}

auto fvkSimpleFaceDetector::loadCascadeClassifier(const std::string& _filename) -> bool
{
	if (m_fd.setFaceCascade(_filename))
	{
		m_filepath = _filename;
		return true;
	}
	m_filepath = "";
	return false;
}

void fvkSimpleFaceDetector::detect(cv::Mat& _frame, const int _frame_delay_in_detection)
{
	if (m_nframes > _frame_delay_in_detection)
	{
		m_fd.detect(_frame);
		m_nframes = 0;
	}
	m_nframes++;
}