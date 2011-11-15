
#include "stdafx.h"
#include "Polyline.h"
#include "common.h"
#include "PrintManager.h"

#include <iomanip>

using namespace std;

void dnl::Polyline::getBoundingRectangle(dnl::Point &lowerLeft, dnl::Point &upperRight) const
{
	if(m_points.size() == 0)
	{
		// There is no bounding box for a polyline without points.
		assert(0);	
	}

	lowerLeft.m_x = upperRight.m_x = m_points[0].m_x;
	lowerLeft.m_y = upperRight.m_y = m_points[0].m_y;

	for(unsigned int i = 1; i < m_points.size(); i++)
	{
		if(m_points[i].m_x < lowerLeft.m_x) 
		{
			lowerLeft.m_x = m_points[i].m_x;
		}
		if(m_points[i].m_x > upperRight.m_x)
		{
			upperRight.m_x = m_points[i].m_x;
		}
		if(m_points[i].m_y < lowerLeft.m_y)
		{
			lowerLeft.m_y = m_points[i].m_y;
		}
		if(m_points[i].m_y > upperRight.m_y)
		{
			upperRight.m_y = m_points[i].m_y;
		}
	}
}

void dnl::Polyline::addPoint(dnl::Point newPoint)
{
	m_points.push_back(newPoint);
}

void dnl::Polyline::output()
{
	cout << fixed << left << setw(40) << m_name << endl;

	for(unsigned int i = 0; i < m_points.size(); i++)
	{
		cout << "Point " << fixed << left << setw(6) << i;
		m_points[i].output();	
	}

	cout << endl;
}

void dnl::Polyline::print(PrintManager &printMan)
{
	for(unsigned int i = 0; i < m_points.size() - 1; i++)
	{
		printMan.PrintLine(m_points[i], m_points[i+1], &printMan.m_solidGreen);
	}
}

void dnl::Polyline::printPolygon(PrintManager &printMan, Colour *colour /*= NULL*/)
{
	if(colour == NULL)
	{
		colour = &printMan.m_seethroughGreen;
	}

	printMan.PrintPolygon(m_points, colour);
}

bool dnl::Polyline::isClosed() const
{
	const unsigned int pointsSize = m_points.size();
	
	if(pointsSize < 2)
	{
		return false;
	}

	const bool isClosed = 
		(m_points[0].m_x == m_points[pointsSize - 1].m_x) &&
		(m_points[0].m_y == m_points[pointsSize - 1].m_y);
}