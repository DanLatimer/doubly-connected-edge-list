
#include "stdafx.h"
#include "Polyline.h"
#include "common.h"
#include "PrintManager.h"

#include <iomanip>

using namespace std;

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

void dnl::Polyline::printPolygon(PrintManager &printMan)
{
	printMan.PrintPolygon(m_points, &printMan.m_seethroughGreen);
}