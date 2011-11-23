
#include "stdafx.h"
#include "PrintManager.h"

#include <objidl.h>
#include <gdiplus.h>
#include "Strip.h"
#include "Point.h"
#include <gdiplus.h>
#include "Point.h"
#include "RunLengthCoding.h"

#include <sstream>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

Color colourToColor(Colour colour)
{
	return Color(colour.m_opacity, colour.m_red, colour.m_green, colour.m_blue);
}

Colour PrintManager::getRandomColour(int opacity)
{
	return Colour(opacity, rand() % 255, rand() % 255, rand() % 255); 
}

PrintManager::PrintManager(
	const dnl::Point & LLWindow,
	const dnl::Point & URWindow,
	const double multX,
	const double multY,
	const int screenWidth,
	const int screenHeight,
	HDC *hdc)
	: m_LLDG(false),
	m_LLWindow(LLWindow),
	m_URWindow(URWindow),
	m_multX(multX),
	m_multY(multY),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight),
	m_hdc(hdc),
	m_graphics(new Gdiplus::Graphics(*hdc)),
	m_solidBlack(255, 0, 0, 0),
	m_solidWhite(255, 255, 255, 255),
	m_solidRed(255, 255, 0, 0),
	m_solidBlue(255, 0, 0, 255),
	m_seeThroughBlue(80, 0, 0, 255),
	m_superSeeThroughBlue(40, 0, 0, 255),
	m_solidGreen(255, 51, 153, 51),
	m_seethroughGreen(80, 51, 153, 51)
{
	m_graphics->SetClip(Rect(0, 0, m_screenWidth, m_screenHeight));
	m_graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
}

float PrintManager::transformX(const double xValue)
{
	return (float)((xValue - m_LLWindow.m_x) * m_multX);
}

float PrintManager::transformY(const double yValue)
{
	if(!m_LLDG)
	{
		return (float)((yValue - m_LLWindow.m_y) * m_multY);
	}
	else
	{
		return (float)((m_LLWindow.m_y - yValue) * m_multY);
	}
}

int PrintManager::transformWidth(const int width)
{
	return (int)(width * m_multX);
}

int PrintManager::transformHeight(const int height)
{
	return (int)(height * m_multX);
}

int PrintManager::transformTextSize(const int size)
{
	return (int)(size * m_multX);
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

	m_graphics->DrawPath(&pen, &path);
	m_graphics->FillPath(&brush, &path);
}

void PrintManager::PrintLine(dnl::Point begin, dnl::Point end, Colour *colour/* =(255, 0, 0, 255)*/, float width /*= 1*/)
{
	if(colour == NULL)
	{
		colour = &m_solidBlue;
	}

	Pen      pen(colourToColor(*colour), width);
	m_graphics->DrawLine(&pen, 
		transformX(begin.m_x), 
		transformY(begin.m_y), 
		transformX(end.m_x), 
		transformY(end.m_y));
}

void PrintManager::PrintText(
	const dnl::Point begin, 
	const std::string &text,
	int size /*= 27*/,
	Colour *colour /*= NULL*/)
{
	if(colour == NULL)
	{
		colour = &m_seeThroughBlue;
	}

	WCHAR currLine[1000] = {0};
	for(unsigned int i = 0; i < text.length(); i++)
	{
		currLine[i] = text.at(i);
	}

	size = transformTextSize(size);

	FontFamily  fontFamily(L"Helvetica");
	Font        font(&fontFamily, (float)size, FontStyleRegular, UnitPixel);
	SolidBrush  solidBrush(colourToColor(*colour));

	PointF pointF(transformX(begin.m_x), transformY(begin.m_y));
	m_graphics->DrawString(currLine, -1, &font, pointF, &solidBrush);
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

	m_graphics->FillPath(&brush, &path);
}

void PrintManager::PrintRunLengthCoding(
		const double minX, 
		const double minY, 
		const double maxX, 
		const double maxY, 
		const RunLengthCoding &runLengthCoding,
		Colour *colour /*= NULL*/)
{
	// Transform from RLC to 2DArray
	std::vector< std::vector<int> > my2DVector;
	
	const unsigned int width = runLengthCoding.m_columns;
	const unsigned int height = runLengthCoding.m_rows;

	// Initialize vector with all 0's
	for(unsigned int i = 0; i < height; i++)
	{
		std::vector<int> my1DVector;
		for(unsigned int j = 0; j < width; j++)
		{
			my1DVector.push_back(0);
		}
		my2DVector.push_back(my1DVector);
	}

	// Mark the black squares with 1's
	bool makeBlack = false;
	int rlcIndex = 0;
	for(unsigned int i = 0; i < height; i++)
	{
		makeBlack = false;
		for(unsigned int j = 0; j < width; j++)
		{
			std::pair<int, int> current = runLengthCoding.m_rlcData[rlcIndex];
			/*if(current.second != i)
			{
				break;
			}*/
			
			if(current.first == j && current.second == i)
			{
				rlcIndex++;
				makeBlack = !makeBlack;
			}

			if(makeBlack)
			{
				my2DVector[i][j] = 1;
			}
		}
	}

	PrintRasterGrid(minX, minY, maxX, maxY, my2DVector, colour);
}

void PrintManager::PrintRasterGrid(
		const double minX, 
		const double minY, 
		const double maxX, 
		const double maxY, 
		const std::vector< std::vector<int> > &my2DVector,
		Colour *colour /*= NULL*/)
{
	assert(my2DVector.size() > 0);
	assert(my2DVector[0].size() > 0);
	// Assume that all 1D Vectors inside the 2D Vector are the same size

	// Start drawing
	if(colour == NULL)
	{
		colour = &m_solidBlue;
	}

	const double width = maxX - minX;
	const double height = maxY - minY;
	const int numWide = my2DVector[0].size();
	const int numHigh = my2DVector.size();
	const double oneBoxWidth = width / numWide;
	const double oneBoxHeight = height / numHigh;

	//Draw the grid!
	PrintGridX(oneBoxWidth, minX, maxX, minY, maxY, colour);
	PrintGridY(oneBoxHeight, minX, maxX, minY, maxY, colour);

	SolidBrush   brush(colourToColor(*colour));

	for(int i = 0; i < numHigh; i++)
	{
		assert(my2DVector[i].size() == numWide);
		for(int j = 0; j < numWide; j++)
		{
			// Get the rectangle to be printed.
			const double x = minX + (oneBoxWidth * j);
			const double y = minY + (oneBoxHeight * i);

			Colour *colourToUse = colour;
		
			// Print the square if it is encoded to be printted
			if(my2DVector[i][j] != 0)
			{
				Rect rectangle(
					transformX(x), 
					transformY(y),
					transformWidth(oneBoxWidth), 
					transformHeight(oneBoxHeight));
				m_graphics->FillRectangle(&brush, rectangle);
				colourToUse = &m_solidWhite;
			}

			// Print coordinates in rectangle
			/*std::stringstream ss;
			ss << "(" << i << ", " << j << ")";
			std::string coords;
			std::getline(ss, coords);


			this->PrintText(dnl::Point(x,y), coords, 27, colourToUse);*/
		}
	}
}
void PrintManager::FillPoint(const dnl::Point &point, Colour *colour /*= NULL*/)
{
	if(colour == NULL)
	{
		colour = &m_solidBlack;
	}

	GraphicsPath path;
	SolidBrush   brush(colourToColor(*colour));

	double size = 1;
	float width = (float)transformWidth(size);
	float height = (float)transformHeight(size);

	path.AddEllipse(
		transformX(point.m_x) - width/2, 
		transformY(point.m_y) - height/2,
		width, 
		height);

	m_graphics->FillPath(&brush, &path);
}

void PrintManager::PrintPoint(const dnl::Point &point, Colour *colour /*= NULL*/)
{
	if(colour == NULL)
	{
		colour = &m_solidBlack;
	}

	GraphicsPath path;
	Pen          pen(colourToColor(*colour), 2);

	double size = 1;
	float width = (float)transformWidth(size);
	float height = (float)transformHeight(size);

	path.AddEllipse(
		transformX(point.m_x) - width/2, 
		transformY(point.m_y) - height/2,
		width, 
		height);

	m_graphics->DrawPath(&pen, &path);
}
