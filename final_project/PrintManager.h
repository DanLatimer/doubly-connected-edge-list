
#pragma once

#include "common.h"
#include "Point.h"
#include <vector>

class Colour
{
public:
	Colour(int opacity, int red, int green, int blue) 
		: m_red(red), m_green(green), m_blue(blue), m_opacity(opacity) { }
	int m_red;
	int m_green;
	int m_blue;
	int m_opacity;
};

class PrintManager
{
public:
	PrintManager(
		const dnl::Point & LLWindow,
		const dnl::Point & URWindow,
		const double multX,
		const double multY,
		const int screenWidth,
		const int screenHeight,
		HDC *hdc);

	// Print methods
	void PrintLine(dnl::Point begin, dnl::Point end, Colour *colour = NULL);
	void PrintArrow(dnl::Point begin, dnl::Point end);
	void PrintPolygon(const std::vector<dnl::Point> &points, Colour *colour = NULL);
	void PrintGridX(
		const double width,  
		const double minX, 
		const double maxX, 
		const double minY, 
		const double maxY,
		Colour *colour = NULL);
	void PrintGridY(
		const double width,  
		const double minX, 
		const double maxX, 
		const double minY, 
		const double maxY,
		Colour *colour = NULL);


private:
	float transformX(const double xValue);
	float transformY(const double yValue);

	// Data members
	HDC *m_hdc;
	dnl::Point m_LLWindow;
	dnl::Point m_URWindow;
	double m_multX;
	double m_multY;
	int m_screenWidth;
	int m_screenHeight;

public:
	Colour m_solidBlue;
	Colour m_seeThroughBlue;
	Colour m_superSeeThroughBlue;
	Colour m_solidGreen;
	Colour m_seethroughGreen;
};

