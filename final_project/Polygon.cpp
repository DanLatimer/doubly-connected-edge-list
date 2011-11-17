
#include "stdafx.h"
#include "common.h"
#include "Polygon.h"
#include "utils.h"

using namespace std;
using namespace dnl;

void Polygon::print(PrintManager &printMan)
{
	// DNL TODO: implement
	
	// TEMPORARY IMPLEMENTATION (JUST PRINT ALL RINGS
	printMan.PrintPolygon(m_outerRing.m_points, &printMan.m_seethroughGreen);

	for(unsigned int i = 0; i < m_innerRings.size(); i++)
	{
		printMan.PrintPolygon(m_innerRings[i].m_points);
	}
}

void Polygon::setOuterRing(const dnl::Polyline &polyline)
{
	m_outerRing = polyline;
}

void Polygon::addInnerRing(const dnl::Polyline &polyline)
{
	m_innerRings.push_back(polyline);
}

bool Polygon::getBoundingRectangle(dnl::Point &lowerLeft, dnl::Point &upperRight)
{
	if(m_outerRing.m_points.size() == 0)
	{
		return false;
	}

	// Only need to traverse outer ring as all 
	// inner rings should be contained within it.
	lowerLeft.m_x = m_outerRing.m_points[0].m_x;
	lowerLeft.m_y = m_outerRing.m_points[0].m_y;
	upperRight.m_x = m_outerRing.m_points[0].m_x;
	upperRight.m_y = m_outerRing.m_points[0].m_y;
	for(unsigned int i = 0; i < m_outerRing.m_points.size(); i++)
	{
		utils::expandBoundingBox(lowerLeft, upperRight, m_outerRing.m_points[0]);
	}
}