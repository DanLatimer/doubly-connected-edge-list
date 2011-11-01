

#pragma once

#include "common.h"
#include "Point.h"
//#include "Polyline.h"

class PrintManager
{
public:
	PrintManager(const int originX, const int originY, HDC *hdc) : m_originX(originX), m_originY(originY), m_hdc(hdc) { }

	// Print methods
	void PrintLine(dnl::Point begin, dnl::Point end);
	void PrintArrow(dnl::Point begin, dnl::Point end);

private:

	// Data members
	HDC *m_hdc;
	int m_originX;
	int m_originY;
};

