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

#include "fvkExport.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <string>

namespace R3D
{

class FVK_EXPORT fvkFaceDetector
{
public:
	fvkFaceDetector(const std::string _cascade_file_path);
	~fvkFaceDetector();

	bool setFaceCascade(const std::string& _cascade_file_path);
	cv::CascadeClassifier* faceCascade() const;
	void setResizedWidth(const int _width);
	int resizedWidth() const;
	cv::Rect face() const;
	cv::Point facePosition() const;
	void setTemplateMatchingMaxDuration(const double _s);
	double templateMatchingMaxDuration() const;
	void detectFaceAllSizes(const cv::Mat& _frame);
	cv::Point detect(cv::Mat& _frame);
	cv::Point operator >> (cv::Mat& _frame);

private:
	cv::Rect doubleRectSize(const cv::Rect &inputRect, const cv::Rect &frameSize) const;
	cv::Rect biggestFace(std::vector<cv::Rect> &faces) const;
	cv::Point centerOfRect(const cv::Rect &rect) const;
	cv::Mat getFaceTemplate(const cv::Mat &frame, cv::Rect face);
	void detectFaceAroundRoi(const cv::Mat &frame);
	void detectFacesTemplateMatching(const cv::Mat &frame);

	static const double TICK_FREQUENCY;

	cv::CascadeClassifier* m_faceCascade = nullptr;
	std::vector<cv::Rect> m_allFaces;
	cv::Rect m_trackedFace;
	cv::Rect m_faceRoi;
	cv::Mat m_faceTemplate;
	cv::Mat m_matchingResult;
	bool m_templateMatchingRunning = false;
	int64 m_templateMatchingStartTime = 0;
	int64 m_templateMatchingCurrentTime = 0;
	bool m_foundFace = false;
	double m_scale;
	int m_resizedWidth = 320;
	cv::Point m_facePosition;
	double m_templateMatchingMaxDuration = 3;
};

class FVK_EXPORT fvkFaceTracker
{
public:
	fvkFaceTracker();

	// Description:
	// Function to load a classifier from a file.
	// filename Name of the file from which the classifier is loaded.
	// It returns true on success.
	bool loadCascadeClassifier(const std::string& _filename);
	// Description:
	// Function to get the file path of the classifier.
	std::string getCascadeClassifierFilePath() const { return m_filepath; }

	// Description:
	// Function to set RGB color to the rectangle of the detected face.
	void setTrackedFaceColor(const cv::Vec3b& _rgb) { _face_rect_color = _rgb; }
	// Description:
	// Function to get RGB color to the rectangle of the detected face.
	cv::Vec3b getTrackedFaceColor() const { return _face_rect_color; }

	// Description:
	// Function to get a reference to face detector.
	fvkFaceDetector& getFaceDetector() { return m_fd; }

	// Description:
	// Function that detect faces, then selects the biggest face, and then start
	// tracking it.
	virtual void execute(cv::Mat& _frame);

protected:
	cv::Vec3b _face_rect_color;
	std::string m_filepath;
	fvkFaceDetector m_fd;
	int nframes;
};

}

#endif // fvkFaceDetector_h__