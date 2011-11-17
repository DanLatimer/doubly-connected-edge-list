
#pragma once

#include "common.h"
#include "Point.h"
#include "RunLengthCoding.h"
#include <vector>

// Forward declarations
namespace Gdiplus{
class Graphics;
}

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

	bool m_LLDG;

	// Print Primitives
	void PrintPoint(const dnl::Point &point, Colour *colour = NULL);
	void FillPoint(const dnl::Point &point, Colour *colour = NULL);
	void PrintLine(dnl::Point begin, dnl::Point end, Colour *colour = NULL, float width = 1);
	void PrintArrow(dnl::Point begin, dnl::Point end);
	void PrintText(const dnl::Point begin, const std::string &text, int size = 27, Colour *colour = NULL);

	// Print Complex Structures
	void PrintPolygon(const std::vector<dnl::Point> &points, Colour *colour = NULL);
	void PrintRasterGrid(
		const double minX, 
		const double minY, 
		const double maxX, 
		const double maxY, 
		const std::vector< std::vector<int> > &my2DVector,
		Colour *colour = NULL);
	void PrintRunLengthCoding(
		const double minX, 
		const double minY, 
		const double maxX, 
		const double maxY, 
		const RunLengthCoding &runLengthCoding,
		Colour *colour = NULL);
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
	int transformWidth(const int width);
	int transformHeight(const int height);
	int transformTextSize(const int size);

	// Data members
	HDC *m_hdc;
	std::auto_ptr<Gdiplus::Graphics> m_graphics;
	dnl::Point m_LLWindow;
	dnl::Point m_URWindow;
	double m_multX;
	double m_multY;
	int m_screenWidth;
	int m_screenHeight;

public:
	Colour m_solidBlack;
	Colour m_solidWhite;
	Colour m_solidRed;
	Colour m_solidGreen;
	Colour m_solidBlue;
	Colour m_seeThroughBlue;
	Colour m_superSeeThroughBlue;
	Colour m_seethroughGreen;
};

