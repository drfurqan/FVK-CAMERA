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

#include <fvk/camera/fvkImagePlot.h>


#if WIN32 
#define snprintf sprintf_s 
#endif 

using namespace R3D;

/************************************************************************/
/* A curve                                                              */
/************************************************************************/
class FVK_CAMERA_EXPORT fvkPlotSeries
{
public:
	fvkPlotSeries();
	fvkPlotSeries(const fvkPlotSeries& s);
	~fvkPlotSeries();
	void clear();

	void set(unsigned int count, float* p);
	void set(cv::Scalar color, bool auto_color = true);
	void set(int r, int g, int b, bool auto_color = true);

	unsigned int m_count;   // number of points
	std::string m_label;    // name of the curve
	float *p_data;			// curve data
	bool m_auto_color;		// 
	cv::Scalar m_color;		// color of the curve
};

/************************************************************************/
/* A figure comprises of several curves                                 */
/************************************************************************/
class FVK_CAMERA_EXPORT fvkPlotFigure
{

public:
	fvkPlotFigure(const std::string& name);
	void clear();

	fvkPlotSeries* add(const fvkPlotSeries& s);

	void drawLabels(cv::Mat& outimg, int posx, int posy);
	void setFigureSize(int w, int h);
	void setBorderSize(int size);
	void setInvertColors(bool invert);
	void setAxisEnabled(bool b);
	void setLabelsEnabled(bool b);

	void calculatePlot();
	std::string getFigureName();

	cv::Mat& getPlottedImage() { return p_plottedimg; }

private:
	void drawAxis(cv::Mat& outimg);
	void drawPlots(cv::Mat& outimg);
	void initialize();
	cv::Scalar getAutoColor();

	cv::Mat p_plottedimg;
	std::string figure_name;
	cv::Size figure_size;

	int border_size;

	cv::Scalar backgroud_color;
	cv::Scalar axis_color;
	cv::Scalar text_color;

	std::vector<fvkPlotSeries> plots;

	bool custom_range_y;
	float y_max;
	float y_min;

	float y_scale;

	bool custom_range_x;
	float x_max;
	float x_min;

	float x_scale;

	int color_index;

	bool isInvert;
	bool isAxis;
	bool isLabls;
};

/************************************************************************/
/* Set Series                                                           */
/************************************************************************/
fvkPlotSeries::fvkPlotSeries() : p_data(nullptr), m_label("")
{
	clear();
}
fvkPlotSeries::fvkPlotSeries(const fvkPlotSeries& s) :
	m_count(s.m_count),
	m_label(s.m_label),
	m_auto_color(s.m_auto_color),
	m_color(s.m_color)
{
	p_data = new float[m_count];
	std::memcpy(p_data, s.p_data, m_count * sizeof(float));
}
fvkPlotSeries::~fvkPlotSeries()
{
	clear();
}
void fvkPlotSeries::clear()
{
	if (p_data != nullptr)
		delete[] p_data;
	p_data = nullptr;

	m_count = 0;
	m_color = CV_RGB(0, 0, 0);
	m_auto_color = true;
	m_label = "";
}
void fvkPlotSeries::set(unsigned int count, float* p)
{
	clear();
	m_count = count;
	p_data = p;
}
void fvkPlotSeries::set(int r, int g, int b, bool auto_color)
{
	r = r > 0 ? r : 0;
	g = g > 0 ? g : 0;
	b = b > 0 ? b : 0;
	m_color = CV_RGB(r, g, b);
	set(m_color, auto_color);
}
void fvkPlotSeries::set(cv::Scalar color, bool auto_color)
{
	m_color = color;
	m_auto_color = auto_color;
}

/************************************************************************/
/* Set Figure                                                           */
/************************************************************************/
fvkPlotFigure::fvkPlotFigure(const std::string& name)
{
	figure_name = name;
	custom_range_y = false;
	custom_range_x = false;
	backgroud_color = CV_RGB(255, 255, 255);
	axis_color = CV_RGB(0, 0, 0);
	text_color = CV_RGB(0, 0, 0);
	figure_size = cv::Size(100, 100);
	border_size = 30;
	plots.reserve(10);
	isInvert = false;
	isAxis = true;
	isLabls = true;
}
void fvkPlotFigure::setAxisEnabled(bool b)
{
	isAxis = b;
}
void fvkPlotFigure::setLabelsEnabled(bool b)
{
	isLabls = b;
}
void fvkPlotFigure::setInvertColors(bool invert)
{
	isInvert = invert;
}
void fvkPlotFigure::setFigureSize(int w, int h)
{
	figure_size = cv::Size(w, h);
}
void fvkPlotFigure::setBorderSize(int size)
{
	border_size = size;
}

std::string fvkPlotFigure::getFigureName(void)
{
	return figure_name;
}

fvkPlotSeries* fvkPlotFigure::add(const fvkPlotSeries& s)
{
	plots.push_back(s);
	return &(plots.back());
}

void fvkPlotFigure::clear()
{
	if (!plots.empty())
		plots.clear();
}

void fvkPlotFigure::initialize()
{
	if (this->isInvert)
	{
		backgroud_color = CV_RGB(0, 0, 0);
		axis_color = CV_RGB(255, 255, 255);
		text_color = CV_RGB(255, 255, 255);
	}
	else
	{
		backgroud_color = CV_RGB(255, 255, 255);
		axis_color = CV_RGB(0, 0, 0);
		text_color = CV_RGB(0, 0, 0);
	}

	color_index = 0;

	// size of the figure
	if (figure_size.width <= border_size * 2 + 100)
		figure_size.width = border_size * 2 + 100;
	if (figure_size.height <= border_size * 2 + 200)
		figure_size.height = border_size * 2 + 200;

	y_max = FLT_MIN;
	y_min = FLT_MAX;

	x_max = 0;
	x_min = 0;

	// find maximum/minimum of axes
	for (auto iter = plots.begin(); iter != plots.end(); ++iter)
	{
		auto count = static_cast<float>(iter->m_count);
		auto p = iter->p_data;
		for (unsigned int i = 0; i < iter->m_count; i++)
		{
			float v = p[i];
			if (v < y_min)
				y_min = v;
			if (v > y_max)
				y_max = v;
		}

		if (x_max < count)
			x_max = count;
	}

	// calculate zoom scale
	// set to 2 if y range is too small
	auto y_range = y_max - y_min;
	auto eps = 1e-9f;
	if (y_range <= eps)
	{
		y_range = 1;
		y_min = y_max / 2;
		y_max = y_max * 3 / 2;
	}

	x_scale = 1.0f;
	if (x_max - x_min > 1)
		x_scale = static_cast<float>(figure_size.width - border_size * 2) / (x_max - x_min);

	y_scale = static_cast<float>(figure_size.height - border_size * 2) / y_range;
}

cv::Scalar fvkPlotFigure::getAutoColor()
{
	// 	change color for each curve.
	cv::Scalar col;

	switch (color_index)
	{
	case 1:
		col = CV_RGB(60, 60, 255);	// light-blue
		break;
	case 2:
		col = CV_RGB(60, 255, 60);	// light-green
		break;
	case 3:
		col = CV_RGB(255, 60, 40);	// light-red
		break;
	case 4:
		col = CV_RGB(0, 210, 210);	// blue-green
		break;
	case 5:
		col = CV_RGB(180, 210, 0);	// red-green
		break;
	case 6:
		col = CV_RGB(210, 0, 180);	// red-blue
		break;
	case 7:
		col = CV_RGB(0, 0, 185);		// dark-blue
		break;
	case 8:
		col = CV_RGB(0, 185, 0);		// dark-green
		break;
	case 9:
		col = CV_RGB(185, 0, 0);		// dark-red
		break;
	default:
		col = CV_RGB(200, 200, 200);	// grey
		color_index = 0;
	}
	color_index++;
	return col;
}

void fvkPlotFigure::drawAxis(cv::Mat& outimg)
{
	auto bs = border_size;
	auto h = figure_size.height;
	auto w = figure_size.width;

	// size of graph
	auto gh = h - bs * 2;
	auto gw = w - bs * 2;

	// draw the horizontal and vertical axis
	// let x, y axies cross at zero if possible.
	auto y_ref = y_min;
	if ((y_max > 0) && (y_min <= 0))
		y_ref = 0;

	int x_axis_pos = h - bs - cvRound((y_ref - y_min) * y_scale);

	cv::line(outimg, cv::Point(bs, x_axis_pos), cv::Point(w - bs, x_axis_pos), axis_color, cv::LINE_AA);
	cv::line(outimg, cv::Point(bs, h - bs), cv::Point(bs, h - bs - gh), axis_color, cv::LINE_AA);

	// Write the scale of the y axis
	//cv::Font font;
	//cv::initFont(&font, cv::FONT_HERSHEY_PLAIN, 0.55, 0.7, 0, 1, cv::AA);

	auto chw = 6, chh = 10;
	char text[16];

	// y max
	if ((y_max - y_ref) > 0.05 * (y_max - y_min))
	{
		snprintf(text, sizeof(text) - 1, "%.1f", y_max);
		cv::putText(outimg, text, cv::Point(bs / 5, bs - chh / 2), cv::FONT_HERSHEY_PLAIN, 0.55, text_color, 1, cv::LINE_AA);
	}
	// y min
	if ((y_ref - y_min) > 0.05 * (y_max - y_min))
	{
		snprintf(text, sizeof(text) - 1, "%.1f", y_min);
		cv::putText(outimg, text, cv::Point(bs / 5, h - bs + chh), cv::FONT_HERSHEY_PLAIN, 0.55, text_color, 1, cv::LINE_AA);
	}

	// x axis
	snprintf(text, sizeof(text) - 1, "%.1f", y_ref);
	cv::putText(outimg, text, cv::Point(bs / 5, x_axis_pos + chh / 2), cv::FONT_HERSHEY_PLAIN, 0.55, text_color, 1, cv::LINE_AA);

	// Write the scale of the x axis
	snprintf(text, sizeof(text) - 1, "%.0f", x_max);
	cv::putText(outimg, text, cv::Point(w - bs - static_cast<int>(std::strlen(text)) * chw, x_axis_pos + chh), cv::FONT_HERSHEY_PLAIN, 0.55, text_color, 1, cv::LINE_AA);

	// x min
	snprintf(text, sizeof(text) - 1, "%.0f", x_min);
	cv::putText(outimg, text, cv::Point(bs, x_axis_pos + chh), cv::FONT_HERSHEY_PLAIN, 0.55, text_color, 1, cv::LINE_AA);
}

void fvkPlotFigure::drawPlots(cv::Mat& outimg)
{
	auto bs = border_size;
	auto h = figure_size.height;
	auto w = figure_size.width;

	// draw the curves
	for (auto iter = plots.begin(); iter != plots.end(); ++iter)
	{
		auto p = iter->p_data;

		// automatically change curve color
		if (iter->m_auto_color == true)
			iter->set(getAutoColor());

		cv::Point prev_point;
		for (unsigned int i = 0; i < iter->m_count; i++)
		{
			int y = cvRound((p[i] - y_min) * y_scale);
			int x = cvRound((i - x_min) * x_scale);
			cv::Point next_point = cv::Point(bs + x, h - (bs + y));
			cv::circle(outimg, next_point, 1, iter->m_color, 1, cv::LINE_AA);

			// draw a line between two points
			if (i >= 1)
				cv::line(outimg, prev_point, next_point, iter->m_color, 1, cv::LINE_AA);
			prev_point = next_point;
		}
	}
}

void fvkPlotFigure::drawLabels(cv::Mat& outimg, int posx, int posy)
{
	// character size
	int chw = 6, chh = 8;

	for (auto iter = plots.begin(); iter != plots.end(); ++iter)
	{
		auto lbl = iter->m_label;
		// draw label if one is available
		if (lbl.length() > 0)
		{
			cv::line(outimg, cv::Point(posx, posy - chh / 2), cv::Point(posx + 15, posy - chh / 2), iter->m_color, 2, cv::LINE_AA);
			cv::putText(outimg, lbl.c_str(), cv::Point(posx + 20, posy), cv::FONT_HERSHEY_PLAIN, 0.55, iter->m_color, 1, cv::LINE_AA);
			posy += int(chh * 1.5);
		}
	}

}

// whole process of draw a figure.
void fvkPlotFigure::calculatePlot()
{
	initialize();
	p_plottedimg = cv::Mat(figure_size, CV_8UC3);
	p_plottedimg.setTo(backgroud_color);
	if (isAxis) drawAxis(p_plottedimg);
	if (isLabls) drawLabels(p_plottedimg, figure_size.width - 40, 25);
	drawPlots(p_plottedimg);
}

/************************************************************************/
/* Start Plot Manager                                                   */
/************************************************************************/
fvkPlotManager::fvkPlotManager() : 
	active_figure(nullptr),
	active_series(nullptr)
{
}

fvkPlotManager::~fvkPlotManager()
{
	for (auto &i : figure_list)
	{
		i->clear();
		delete i;
	}
}
// search a named window, return null if not found.
fvkPlotFigure* fvkPlotManager::findFigure(const std::string& name)
{
	for (auto &i : figure_list)
	{
		if (i->getFigureName() == name)
			return i;
	}
	return nullptr;
}

// plot a new curve, if a figure of the specified figure name already exists,
// the curve will be plot on that figure; if not, a new figure will be created.
void fvkPlotManager::plotData(const std::string& fig_name, const float *p, int count, int step, int R, int G, int B)
{
	if (count < 1)
		return;

	if (step <= 0)
		step = 1;

	// copy data and create a series format.
	auto data_copy = new float[count];

	for (auto i = 0; i < count; i++)
		*(data_copy + i) = *(p + step * i);

	fvkPlotSeries s;
	s.set(count, data_copy);

	if ((R > 0) || (G > 0) || (B > 0))
		s.set(R, G, B, false);

	// search the named window and create one if none was found
	active_figure = findFigure(fig_name);
	if (active_figure == nullptr)
	{
		figure_list.push_back(new fvkPlotFigure(fig_name));
		active_figure = findFigure(fig_name);
		if (active_figure == nullptr)
			return;
	}

	active_series = active_figure->add(s);
}


// plot a new curve, if a figure of the specified figure name already exists,
// the curve will be plot on that figure; if not, a new figure will be created.
// static method
template <typename _T>
void fvkPlotManager::plot(const std::string& fig_name, const _T* data, int count, int step, int R, int G, int B)
{
	if (step <= 0) 
		step = 1;

	auto data_copy = new float[count * step];
	auto dst = data_copy;
	const _T* src = data;
	for (int i = 0; i < count * step; i++)
	{
		*dst = static_cast<float>(*src);
		dst++;
		src++;
	}
	this->plotData(fig_name, data_copy, count, step, R, G, B);
	delete[] data_copy;
}
void fvkPlotManager::calculatePlot() const
{
	active_figure->calculatePlot();
}

template void fvkPlotManager::plot(const std::string& figure_name, const char* p, int count, int step, int R, int G, int B);
template void fvkPlotManager::plot(const std::string& figure_name, const unsigned char* p, int count, int step, int R, int G, int B);
template void fvkPlotManager::plot(const std::string& figure_name, const int* p, int count, int step, int R, int G, int B);
template void fvkPlotManager::plot(const std::string& figure_name, const unsigned int* p, int count, int step, int R, int G, int B);
template void fvkPlotManager::plot(const std::string& figure_name, const float* p, int count, int step, int R, int G, int B);
template void fvkPlotManager::plot(const std::string& figure_name, const double* p, int count, int step, int R, int G, int B);


// delete all plots on a specified figure
void fvkPlotManager::clear(const std::string& name)
{
	auto fig = findFigure(name);
	if (fig) fig->clear();
}
void fvkPlotManager::setLabel(const std::string& l) const
{
	if (active_series != nullptr && active_figure != nullptr)
	{
		active_series->m_label = l;
		active_figure->calculatePlot();
	}
}

/*******************************************************************************/
cv::Mat fvkPlotManager::getPlottedImage(const std::string& name)
{
	auto fig = findFigure(name);
	if (fig) return fig->getPlottedImage();
	return cv::Mat();
}
void fvkPlotManager::setPlotSize(const std::string& name, int w, int h)
{
	auto fig = findFigure(name);
	if (fig) fig->setFigureSize(w, h);
}
void fvkPlotManager::setBorderSize(const std::string& name, int size)
{
	auto fig = findFigure(name);
	if (fig) fig->setBorderSize(size);
}
void fvkPlotManager::invertPlotColor(const std::string& name, bool invert)
{
	auto fig = findFigure(name);
	if (fig) fig->setInvertColors(invert);
}

void fvkPlotManager::setAxisEnabled(const std::string& name, bool isOn)
{
	auto fig = findFigure(name);
	if (fig) fig->setAxisEnabled(isOn);
}
void fvkPlotManager::setLabelsEnabled(const std::string& name, bool isOn)
{
	auto fig = findFigure(name);
	if (fig) fig->setLabelsEnabled(isOn);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
fvkImagePlot::fvkImagePlot() :
	m_plotsize(cv::Size(640, 480)),
	m_row_num(-1),
	m_border_size(30),
	m_isRedCurve(false),
	m_isGreenCurve(false),
	m_isBlueCurve(false),
	m_isInverted(false),
	m_isAxis(true),
	m_isLabel(true),
	m_curve_color(cv::Scalar(150, 150, 150))
{

}

cv::Mat fvkImagePlot::getPlottedImage(cv::Mat& inImg, int plotsize_w, int plotsize_h, int row_number, int border_size, bool isRedCurve, bool isGreenCurve, bool isBlueCurve, bool isRGBCurve,
	bool isInverted, bool isAxis, bool isLabel, int red, int green, int blue, std::string plotname, std::string plotLabel)
{
	if (inImg.empty()) 
		return cv::Mat();

	cv::Mat img;
	if (inImg.depth() != CV_8U)
		inImg.convertTo(img, CV_8U, 255);
	else
		img = inImg.clone();

	if (img.channels() == 1)
		cv::cvtColor(img, img, cv::ColorConversionCodes::COLOR_GRAY2BGR);
	else if (img.channels() == 4)
		cv::cvtColor(img, img, cv::ColorConversionCodes::COLOR_BGRA2BGR);

	if (plotsize_w < 0) plotsize_w = 640;
	if (plotsize_h < 0) plotsize_h = 480;
	if (row_number < 0) row_number = static_cast<int>(img.rows / 2);

	fvkPlotManager pm;

	if (isRGBCurve)
	{
		uchar* pb = img.data + row_number * img.step;
		pm.plot(plotname, pb + 2, img.cols, 3, 200, 0, 0);
		if (isLabel) pm.setLabel("B");
		pm.plot(plotname, pb + 1, img.cols, 3, 0, 200, 0);
		if (isLabel) pm.setLabel("G");
		pm.plot(plotname, pb + 0, img.cols, 3, 0, 0, 200);
		if (isLabel) pm.setLabel("R");
	}
	else
	{
		std::vector<cv::Mat> bgr(img.channels());
		cv::split(img, bgr);

		cv::Mat t;
		if (isRedCurve) t = bgr[2];
		else if (isGreenCurve) t = bgr[1];
		else if (isBlueCurve) t = bgr[0];
		uchar* pb = t.data + row_number * t.step;
		pm.plot(plotname, pb, t.cols, 1, red, green, blue);
	}

	pm.setPlotSize(plotname, plotsize_w, plotsize_h);
	pm.setBorderSize(plotname, border_size);
	pm.invertPlotColor(plotname, isInverted);
	pm.setAxisEnabled(plotname, isAxis);
	pm.setLabelsEnabled(plotname, isLabel);

	if (isRedCurve) pm.setLabel("R");
	else if (isGreenCurve) pm.setLabel("G");
	else if (isBlueCurve) pm.setLabel("B");

	if (isLabel && !isRGBCurve && !isRedCurve && !isGreenCurve && !isBlueCurve) 
		pm.setLabel(plotLabel);

	pm.calculatePlot();

	cv::Mat I = pm.getPlottedImage(plotname);
	if (!I.empty())
	{
		if (img.size() != I.size())
		{
			img = cv::Mat(I.size(), I.depth());
			cv::resize(I, img, I.size(), cv::INTER_LINEAR);
		}
		else
		{
			img = I.clone();
		}
		pm.clear(plotname);
		return img;
	}

	pm.clear(plotname);
	return cv::Mat();
}

void fvkImagePlot::plot(cv::Mat& img)
{
	auto m = getPlottedImage(
		img,
		m_plotsize.width,
		m_plotsize.height,
		m_row_num,
		m_border_size,
		m_isRedCurve,
		m_isGreenCurve,
		m_isBlueCurve,
		!m_isRedCurve && !m_isGreenCurve && !m_isBlueCurve,
		m_isInverted,
		m_isAxis,
		m_isLabel,
		static_cast<int>(m_curve_color[0]),
		static_cast<int>(m_curve_color[1]),
		static_cast<int>(m_curve_color[2]),
		"RGB",
		"RGB Plot");

	if (!m.empty())
		img = m;
}