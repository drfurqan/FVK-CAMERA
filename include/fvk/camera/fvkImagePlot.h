#pragma once
#ifndef fvkImagePlot_h__
#define fvkImagePlot_h__

/********************************************************************
created:	2013/12/26
created:	26:12:2013   22:30
filename: 	fvkImagePlot.h
file base:	fvkImagePlot
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
	
purpose:	Class to draw RGB plot of an image using OpenCV.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkCameraExport.h"

#include <opencv2/opencv.hpp>

class fvkPlotFigure;
class fvkPlotSeries;

namespace R3D
{

class FVK_CAMERA_EXPORT fvkPlotManager
{
public:
	// Description:
	// Default constructor to initiate the object.
	fvkPlotManager();
	// Description:
	// Destructor that destorys data.
	~fvkPlotManager();
	// Description:
	// Function to clear the data.
	void clear(const std::string& _fig_name);

	// Description:
	// Function to set label.
	void setLabel(const std::string& _l) const;
	// Description:
	// Function to set the plotted image size.
	void setPlotSize(const std::string& _name, int _w, int _h);
	// Description:
	// Function to set the border size.
	void setBorderSize(const std::string& _name, int _size);
	// Description:
	// If it's true the background and the curves color will be inverted.
	void invertPlotColor(const std::string& _name, bool _invert);
	// Description:
	// If it's true X and Y axes are visible.
	void setAxisEnabled(const std::string& _name, bool _b);
	// Description:
	// If it's true labels are visible.
	void setLabelsEnabled(const std::string& _name, bool _b);
	// Description:
	// Function to plot the input _data.
	template <typename _T>
	void plot(const std::string& _fig_name, const _T* _data, int _count, int _step, int _R, int _G, int _B);
	// Description:
	// Function to calculate the plot.
	void calculatePlot() const;

	// Description:
	// Function to get the plotted image.
	cv::Mat getPlottedImage(const std::string& _name);

private:
	fvkPlotFigure* findFigure(const std::string& _name);
	void plotData(const std::string& _fig_name, const float* _p, int _count, int _step, int _R, int _G, int _B);

	fvkPlotFigure *active_figure;
	std::vector<fvkPlotFigure*> figure_list;
	fvkPlotSeries *active_series;
};

class FVK_CAMERA_EXPORT fvkImagePlot
{
public:
	// Description:
	// Default constructor to initiate the object.
	fvkImagePlot();

	// Description:
	// Function to get the plotted image.
	// Input _img is the both source and destination.
	void plot(cv::Mat& _img);

	// Description:
	// Function to set the plotted image size.
	void setPlotSize(const cv::Size& val) { m_plotsize = val; }
	// Description:
	// Function to get the plotted image size.
	auto getPlotSize() const { return m_plotsize; }

	// Description:
	// Function to set the row number of the pixels to be plotted.
	void setRowNum(int val) { m_row_num = val; }
	// Description:
	// Function to get the row number of the pixels to be plotted.
	auto getRowNum() const { return m_row_num; }

	// Description:
	// Function to set the border size.
	void setBorderSize(int val) { m_border_size = val; }
	// Description:
	// Function to get the border size.
	auto getBorderSize() const { return m_border_size; }

	// Description:
	// If it's true only red curve will be plotted.
	void setRedCurveEnable(bool val) { m_isRedCurve = val; }
	// Description:
	// Function that returns true if only red curve is being plotted.
	auto isRedCurveEnable() const { return m_isRedCurve; }

	// Description:
	// If it's true only green curve will be plotted.
	void setGreenCurveEnabled(bool val) { m_isGreenCurve = val; }
	// Description:
	// Function that returns true if only green curve is being plotted.
	auto isGreenCurveEnabled() const { return m_isGreenCurve; }

	// Description:
	// If it's true only blue curve will be plotted.
	void setBlueCurveEnabled(bool val) { m_isBlueCurve = val; }
	// Description:
	// Function that returns true if only blue curve is being plotted.
	auto isBlueCurveEnabled() const { return m_isBlueCurve; }

	// Description:
	// If it's true the background and the curves color will be inverted.
	void invert(bool val) { m_isInverted = val; }
	// Description:
	// Function that returns true if the inversion is enabled.
	auto isInverted() const { return m_isInverted; }

	// Description:
	// If it's true X and Y axes are visible.
	void setAxisEnabled(bool val) { m_isAxis = val; }
	// Description:
	// Function that returns true if axes are visible.
	auto isAxisEnabled() const { return m_isAxis; }

	// Description:
	// If it's true labels are visible.
	void setLabelEnabled(bool val) { m_isLabel = val; }
	// Description:
	// Function that returns true if labels are visible.
	auto isLabelEnabled() const { return m_isLabel; }

	// Description:
	// Function to set the curve color.
	void setCurveColor(cv::Scalar val) { m_curve_color = val; }
	// Description:
	// Function to get the curve color.
	auto getCurveColor() const { return m_curve_color; }

	// Description:
	// Function to get the plotted image of the input image. 
	// Returned image should be released after usage.
	static cv::Mat getPlottedImage(
		const cv::Mat& Inimg,								// (Mandatory) Input image (supported channels are 1,3,4).
		int _plotsize_w = -1,						// (Optional) Specify the width of the plot. If not specified, then the plot size will be the size of ImageBox.
		int _plotsize_h = -1,						// (Optional) Specify the height of the plot. If not specified, then the plot size will be the size of ImageBox.
		int _row_number = -1,						// (Optional) Specify the row number. If not specified, then centered row of an image will be plotted.
		int _border_size = 30,						// (Optional) Specify the border size.
		bool _isRedCurve = false,					// (Optional) Set true, if you want to plot only red color curve.
		bool _isGreenCurve = false,					// (Optional) Set true, if you want to plot only green color curve.
		bool _isBlueCurve = false,					// (Optional) Set true, if you want to plot only blue color curve.
		bool _isRGBCurve = true,					// (Optional) Set true, if you want to plot RGB color curves.
		bool _isInverted = false,					// (Optional) Set true, if you want to invert the plot background and text color.
		bool _isAxis = true,						// (Optional) Set true, if you want to plot axis with curves.
		bool _isLabel = true,						// (Optional) Set true, if you want to plot the labels of curves.
		int _R = 150, int _G = 150, int _B = 150,	// (Optional) Set curve color (Red, Green, Blue).
		std::string _Plotname = "RGB",				// (Optional) Name of the plot.
		std::string _PlotLabel = "RGB Plot");		// (Optional) Name of the curve label.

private:
	cv::Size m_plotsize;
	int m_row_num;
	int m_border_size;
	bool m_isRedCurve;
	bool m_isGreenCurve;
	bool m_isBlueCurve;
	bool m_isInverted;
	bool m_isAxis;
	bool m_isLabel;
	cv::Scalar m_curve_color;
};

}

#endif // fvkImagePlot_h__