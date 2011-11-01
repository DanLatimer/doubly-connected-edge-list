
#include "stdafx.h"
#include "Polyline.h"
#include "common.h"

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