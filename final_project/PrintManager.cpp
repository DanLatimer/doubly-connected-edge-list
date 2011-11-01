
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

REAL doubleToReal(const double value, const double zoom)
{
	return (float)(value * zoom);
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
		//dnl::Point perpUnitVectorEB(unitVectorEB.m_y * -1, unitVectorEB.m_x);
		dnl::Point pointA = (unitVectorEB * arrowLength) + end;

		dnl::Point vectorAE = end - pointA;
		dnl::Point unitVectorAE = vectorAE.getUnitVector();
		dnl::Point perpUnitVectorAE(unitVectorAE.m_y * -1, unitVectorAE.m_x);

		AL = (perpUnitVectorAE * arrowWidth) + pointA;
		AR = (perpUnitVectorAE * -arrowWidth) + pointA;
	}

	// DRAW triangle { AL, AR, END }


	Graphics graphics(*m_hdc);

	GraphicsPath path;
	Pen          pen(Color(255, 51, 102, 255), 3);
	SolidBrush   brush(Color(80, 51, 204, 255));

	path.AddLine(
		doubleToReal(AL.m_x + m_originX, m_zoom), 
		doubleToReal(AL.m_y + m_originY, m_zoom), 
		doubleToReal(AR.m_x + m_originX, m_zoom), 
		doubleToReal(AR.m_y + m_originY, m_zoom));
	
	path.AddLine(
		doubleToReal(AR.m_x + m_originX, m_zoom), 
		doubleToReal(AR.m_y + m_originY, m_zoom), 
		doubleToReal(end.m_x + m_originX, m_zoom), 
		doubleToReal(end.m_y + m_originY, m_zoom));
	
	path.AddLine(
		doubleToReal(end.m_x + m_originX, m_zoom), 
		doubleToReal(end.m_y + m_originY, m_zoom), 
		doubleToReal(AL.m_x + m_originX, m_zoom), 
		doubleToReal(AL.m_y + m_originY, m_zoom));

	path.AddLine(
		doubleToReal(AL.m_x + m_originX, m_zoom), 
		doubleToReal(AL.m_y + m_originY, m_zoom), 
		doubleToReal(AR.m_x + m_originX, m_zoom), 
		doubleToReal(AR.m_y + m_originY, m_zoom));

	graphics.DrawPath(&pen, &path);
	graphics.FillPath(&brush, &path);
}

void PrintManager::PrintLine(dnl::Point begin, dnl::Point end)
{
	Graphics graphics(*m_hdc);

	Pen      pen(Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, 
		doubleToReal(begin.m_x + m_originX, m_zoom), 
		doubleToReal(begin.m_y + m_originY, m_zoom), 
		doubleToReal(end.m_x + m_originX, m_zoom), 
		doubleToReal(end.m_y + m_originY, m_zoom));
}
