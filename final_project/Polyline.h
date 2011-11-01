
#ifndef POLYLINE_H
#define POLYLINE_H

#include "common.h"
#include "Point.h"
#include <vector>

namespace dnl
{

class Polyline
{
public:
	Polyline(std::string name) : m_name(name) { }
	//Polyline(const &Polyline other) : m_points(other.m_points) { }

	void addPoint(dnl::Point newPoint);
	void output();

	// Data
	std::string m_name;
	std::vector<dnl::Point> m_points;

};

}

#endif