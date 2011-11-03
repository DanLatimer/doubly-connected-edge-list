
#include "stdafx.h"
#include "PrintManager.h"

#include <objidl.h>
#include <gdiplus.h>
#include "Strip.h"
#include "Point.h"
#include <gdiplus.h>
#include "Point.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

Color colourToColor(Colour colour)
{
	return Color(colour.m_opacity, colour.m_red, colour.m_green, colour.m_blue);
}

PrintManager::PrintManager(
	const dnl::Point & LLWindow,
	const dnl::Point & URWindow,
	const double multX,
	const double multY,
	const int screenWidth,
	const int screenHeight,
	HDC *hdc)
	: m_LLWindow(LLWindow),
	m_URWindow(URWindow),
	m_multX(multX),
	m_multY(multY),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight),
	m_hdc(hdc),
	m_solidBlue(255, 0, 0, 255),
	m_seeThroughBlue(80, 0, 0, 255),
	m_superSeeThroughBlue(40, 0, 0, 255),
	m_solidGreen(255, 51, 153, 51),
	m_seethroughGreen(80, 51, 153, 51)
{

}

float PrintManager::transformX(const double xValue)
{
	return (float)((xValue - m_LLWindow.m_x) * m_multX);
}

float PrintManager::transformY(const double yValue)
{
	return (float)((yValue - m_LLWindow.m_y) * m_multY);
}

void PrintManager::PrintArrow(dnl::Point begin, dnl::Point end)
{
	int arrowSize = 2;
	int arrowLength = 5 * arrowSize;
	int arrowWidth = 3 * arrowSize;

	dnl::Point AL, AR;
	{
		dnl::Point vectorEB = begin - end;
		dnl::Point unitVectorEB = vectorEB.getUnitVector();
		dnl::Point pointA = (unitVectorEB * arrowLength) + end;

		dnl::Point vectorAE = end - pointA;
		dnl::Point unitVectorAE = vectorAE.getUnitVector();
		dnl::Point perpUnitVectorAE(unitVectorAE.m_y * -1, unitVectorAE.m_x);

		AL = (perpUnitVectorAE * arrowWidth) + pointA;
		AR = (perpUnitVectorAE * -arrowWidth) + pointA;
	}

	// DRAW triangle { AL, AR, END }

	Graphics graphics(*m_hdc);
	graphics.SetClip(Rect(0, 0, m_screenWidth, m_screenHeight));

	GraphicsPath path;
	Pen          pen(Color(255, 51, 102, 255), 3);
	SolidBrush   brush(Color(80, 51, 204, 255));

	path.AddLine(
		transformX(AL.m_x), 
		transformY(AL.m_y), 
		transformX(AR.m_x), 
		transformY(AR.m_y));
	
	path.AddLine(
		transformX(AR.m_x), 
		transformY(AR.m_y), 
		transformX(end.m_x), 
		transformY(end.m_y));
	
	path.AddLine(
		transformX(end.m_x), 
		transformY(end.m_y), 
		transformX(AL.m_x), 
		transformY(AL.m_y));

	path.AddLine(
		transformX(AL.m_x), 
		transformY(AL.m_y), 
		transformX(AR.m_x), 
		transformY(AR.m_y));

	graphics.DrawPath(&pen, &path);
	graphics.FillPath(&brush, &path);
}

void PrintManager::PrintLine(dnl::Point begin, dnl::Point end, Colour *colour/* =(255, 0, 0, 255)*/)
{
	if(colour == NULL)
	{
		colour = &m_solidBlue;
	}

	Graphics graphics(*m_hdc);
	graphics.SetClip(Rect(0, 0, m_screenWidth, m_screenHeight));

	Pen      pen(colourToColor(*colour));
	graphics.DrawLine(&pen, 
		transformX(begin.m_x), 
		transformY(begin.m_y), 
		transformX(end.m_x), 
		transformY(end.m_y));
}

void PrintManager::PrintGridX(
	const double width, 
	const double minX, 
	const double maxX, 
	const double minY, 
	const double maxY, 
	Colour *colour /* = NULL*/)
{
	if(colour == NULL)
	{
		colour = &m_seeThroughBlue;
	}

	for(double x = minX; x <= maxX; x += width)
	{
		dnl::Point begin(x, minY);
		dnl::Point end(x, maxY);
		PrintLine(begin, end, colour);
	}
}

void PrintManager::PrintGridY(
	const double width,  
	const double minX, 
	const double maxX, 
	const double minY, 
	const double maxY,
	Colour *colour /* = NULL*/)
{
	if(colour == NULL)
	{
		colour = &m_seeThroughBlue;
	}

	for(double y = minY; y <= maxY; y += width)
	{
		dnl::Point begin(minX, y);
		dnl::Point end(maxX, y);
		PrintLine(begin, end, colour);
	}
}

void PrintManager::PrintPolygon(const std::vector<dnl::Point> &points, Colour *colour /*= NULL*/)
{
	// It better be a closed polygon, so help me if you gave me otherwise...

	// This doesn't really check it's a closed polygon, 
	// but at least it checks that the last point is the first
	//assert(points[0] == points[points.size - 1]); 

	// Start drawing
	if(colour == NULL)
	{
		colour = &m_solidBlue;
	}

	Graphics graphics(*m_hdc);
	graphics.SetClip(Rect(0, 0, m_screenWidth, m_screenHeight));
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeLowQuality);

	SolidBrush   brush(colourToColor(*colour));
	GraphicsPath path;

	path.StartFigure();
	for(unsigned int i = 0; i < points.size() - 1; i++)
	{
		path.AddLine(
			transformX(points[i].m_x), 
			transformY(points[i].m_y), 
			transformX(points[i+1].m_x), 
			transformY(points[i+1].m_y));
	}
	path.CloseFigure();

	graphics.FillPath(&brush, &path);
}