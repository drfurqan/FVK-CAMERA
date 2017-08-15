/*********************************************************************************
created:	2014/05/12   11:01AM
modified:	2017/02/09   11:54PM
filename: 	fvkVideoWriter.cpp
file base:	fvkVideoWriter
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Thread safe class to create a video file using OpenCV video writer.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkVideoWriter.h>

using namespace R3D;

fvkVideoWriter::fvkVideoWriter() :
m_file(std::string("")),
m_size(cv::Size(640, 480)),
m_fps(25),
m_iscolor(true),
m_autocodec(false),
m_codec(std::string("H264"))
{
	m_writer.set(cv::VideoWriterProperties::VIDEOWRITER_PROP_QUALITY, 100.0);
}
fvkVideoWriter::~fvkVideoWriter()
{
	stop();
}

int fvkVideoWriter::open()
{
	if (m_writer.isOpened()) return -1;

	if (m_autocodec)
	{
		m_writer.open(m_file, -1, m_fps, m_size, m_iscolor);
	}
	else
	{
		if (m_codec.length() != 4) return -2;

		m_writer.open(m_file, cv::VideoWriter::fourcc(m_codec[0], m_codec[1], m_codec[2], m_codec[3]), m_fps, m_size, m_iscolor);
	}

	if (!m_writer.isOpened()) return 0;

	return 1;
}

bool fvkVideoWriter::isOpened()
{
	return m_writer.isOpened();
}
void fvkVideoWriter::stop()
{
	if (m_writer.isOpened())
		m_writer.release();
}

void fvkVideoWriter::addFrame(const cv::Mat& _frame)
{
	// _frame must have the same size as has been specified when opening the video writer.
	if (_frame.empty() || _frame.size() != m_size) return;
	m_writer.write(_frame);
}