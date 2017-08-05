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

fvkFaceDetector::fvkFaceDetector(const std::string _cascade_file_path)
{
	setFaceCascade(_cascade_file_path);
}

bool fvkFaceDetector::setFaceCascade(const std::string& _cascade_file_path)
{
	if (_cascade_file_path.empty()) return false;

    if (m_faceCascade == nullptr)
		m_faceCascade = new cv::CascadeClassifier(_cascade_file_path);
    else
		m_faceCascade->load(_cascade_file_path);

	if (m_faceCascade->empty())
	{
		std::cerr << "ERROR: couldn't create cascade classifier. Make sure the file exists." << std::endl;
		return false;
	}
	return true;
}

cv::CascadeClassifier *fvkFaceDetector::faceCascade() const
{
    return m_faceCascade;
}

void fvkFaceDetector::setResizedWidth(const int _width)
{
    m_resizedWidth = std::max(_width, 1);
}

int fvkFaceDetector::resizedWidth() const
{
    return m_resizedWidth;
}

cv::Rect fvkFaceDetector::face() const
{
    cv::Rect faceRect = m_trackedFace;
	faceRect.x = static_cast<int>(faceRect.x / m_scale);
	faceRect.y = static_cast<int>(faceRect.y / m_scale);
	faceRect.width = static_cast<int>(faceRect.width / m_scale);
	faceRect.height = static_cast<int>(faceRect.height / m_scale);
    return faceRect;
}

cv::Point fvkFaceDetector::facePosition() const
{
	return cv::Point(static_cast<int>(m_facePosition.x / m_scale), static_cast<int>(m_facePosition.y / m_scale));
}

void fvkFaceDetector::setTemplateMatchingMaxDuration(const double _s)
{
    m_templateMatchingMaxDuration = _s;
}

double fvkFaceDetector::templateMatchingMaxDuration() const
{
    return m_templateMatchingMaxDuration;
}

fvkFaceDetector::~fvkFaceDetector()
{
    if (m_faceCascade) delete m_faceCascade;
}

cv::Rect fvkFaceDetector::doubleRectSize(const cv::Rect& inputRect, const cv::Rect& frameSize) const
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

cv::Point fvkFaceDetector::centerOfRect(const cv::Rect& rect) const
{
    return cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

cv::Rect fvkFaceDetector::biggestFace(std::vector<cv::Rect>& faces) const
{
    assert(!faces.empty());

    cv::Rect *biggest = &faces[0];
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
cv::Mat fvkFaceDetector::getFaceTemplate(const cv::Mat& _frame, cv::Rect _face)
{
    _face.x += _face.width / 4;
    _face.y += _face.height / 4;
    _face.width /= 2;
    _face.height /= 2;

    cv::Mat faceTemplate = _frame(_face).clone();
    return faceTemplate;
}

void fvkFaceDetector::detectFaceAllSizes(const cv::Mat& frame)
{
    // Minimum face size is 1/5th of screen height
    // Maximum face size is 2/3rds of screen height
    m_faceCascade->detectMultiScale(
		frame, m_allFaces, 1.1, 3, 0,
        cv::Size(frame.rows / 5, frame.rows / 5),
        cv::Size(frame.rows * 2 / 3, frame.rows * 2 / 3));

    if (m_allFaces.empty()) return;

    m_foundFace = true;

    // Locate biggest face
    m_trackedFace = biggestFace(m_allFaces);

    // Copy face template
    m_faceTemplate = getFaceTemplate(frame, m_trackedFace);

    // Calculate roi
    m_faceRoi = doubleRectSize(m_trackedFace, cv::Rect(0, 0, frame.cols, frame.rows));

    // Update face position
    m_facePosition = centerOfRect(m_trackedFace);
}

void fvkFaceDetector::detectFaceAroundRoi(const cv::Mat& frame)
{
    // Detect faces sized +/-20% off biggest face in previous search
    m_faceCascade->detectMultiScale(
		frame(m_faceRoi), m_allFaces, 1.1, 3, 0,
        cv::Size(m_trackedFace.width * 8 / 10, m_trackedFace.height * 8 / 10),
        cv::Size(m_trackedFace.width * 12 / 10, m_trackedFace.width * 12 / 10));

    if (m_allFaces.empty())
    {
        // Activate template matching if not already started and start timer
        m_templateMatchingRunning = true;
        if (m_templateMatchingStartTime == 0)
            m_templateMatchingStartTime = cv::getTickCount();
        return;
    }

    // Turn off template matching if running and reset timer
    m_templateMatchingRunning = false;
    m_templateMatchingCurrentTime = m_templateMatchingStartTime = 0;

    // Get detected face
    m_trackedFace = biggestFace(m_allFaces);

    // Add roi offset to face
    m_trackedFace.x += m_faceRoi.x;
    m_trackedFace.y += m_faceRoi.y;

    // Get face template
    m_faceTemplate = getFaceTemplate(frame, m_trackedFace);

    // Calculate roi
    m_faceRoi = doubleRectSize(m_trackedFace, cv::Rect(0, 0, frame.cols, frame.rows));

    // Update face position
    m_facePosition = centerOfRect(m_trackedFace);
}

void fvkFaceDetector::detectFacesTemplateMatching(const cv::Mat& frame)
{
    // Calculate duration of template matching
    m_templateMatchingCurrentTime = cv::getTickCount();
    double duration = (double)(m_templateMatchingCurrentTime - m_templateMatchingStartTime) / TICK_FREQUENCY;

    // If template matching lasts for more than 2 seconds face is possibly lost
    // so disable it and redetect using cascades
    if (duration > m_templateMatchingMaxDuration) 
	{
        m_foundFace = false;
        m_templateMatchingRunning = false;
        m_templateMatchingStartTime = m_templateMatchingCurrentTime = 0;
    }

    // Template matching with last known face 
    //cv::matchTemplate(frame(m_faceRoi), m_faceTemplate, m_matchingResult, CV_TM_CCOEFF);
    cv::matchTemplate(frame(m_faceRoi), m_faceTemplate, m_matchingResult, CV_TM_SQDIFF_NORMED);
    cv::normalize(m_matchingResult, m_matchingResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    double min, max;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(m_matchingResult, &min, &max, &minLoc, &maxLoc);

    // Add roi offset to face position
    minLoc.x += m_faceRoi.x;
    minLoc.y += m_faceRoi.y;

    // Get detected face
    //m_trackedFace = cv::Rect(maxLoc.x, maxLoc.y, m_trackedFace.width, m_trackedFace.height);
    m_trackedFace = cv::Rect(minLoc.x, minLoc.y, m_faceTemplate.cols, m_faceTemplate.rows);
    m_trackedFace = doubleRectSize(m_trackedFace, cv::Rect(0, 0, frame.cols, frame.rows));

    // Get new face template
    m_faceTemplate = getFaceTemplate(frame, m_trackedFace);

    // Calculate face roi
    m_faceRoi = doubleRectSize(m_trackedFace, cv::Rect(0, 0, frame.cols, frame.rows));

    // Update face position
    m_facePosition = centerOfRect(m_trackedFace);
}

cv::Point fvkFaceDetector::detect(cv::Mat& _frame)
{
    // Downscale frame to m_resizedWidth width - keep aspect ratio
	m_scale = static_cast<double>(std::min(m_resizedWidth, _frame.cols) / static_cast<double>(_frame.cols));
	cv::Size resizedFrameSize = cv::Size(static_cast<int>(m_scale*_frame.cols), static_cast<int>(m_scale*_frame.rows));

    cv::Mat resizedFrame;
    cv::resize(_frame, resizedFrame, resizedFrameSize);

	if (!m_foundFace)
	{
		detectFaceAllSizes(resizedFrame); // Detect using cascades over whole image
	}
    else 
	{
        detectFaceAroundRoi(resizedFrame); // Detect using cascades only in ROI
        if (m_templateMatchingRunning) 
            detectFacesTemplateMatching(resizedFrame); // Detect using template matching
    }

    return m_facePosition;
}

cv::Point fvkFaceDetector::operator >> (cv::Mat& _frame)
{
	return detect(_frame);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define DELAY_IN_FACE_DETECTION 5

fvkFaceTracker::fvkFaceTracker() : 
_face_rect_color(cv::Vec3b(166, 154, 75)),
m_filepath("haarcascade_frontalface_default.xml"),
m_fd(""),
nframes(0)
{
}

bool fvkFaceTracker::loadCascadeClassifier(const std::string& _filename)
{
	m_filepath = _filename;
	return m_fd.setFaceCascade(_filename);
}

void fvkFaceTracker::execute(cv::Mat& _frame)
{
	cv::rectangle(_frame, m_fd.face(), _face_rect_color);
	if (nframes > DELAY_IN_FACE_DETECTION)
	{
		m_fd.detect(_frame);
		nframes = 0;
	}
	nframes++;
}