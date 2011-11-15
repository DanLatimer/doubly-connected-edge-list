
#pragma once

#include "common.h"
#include "Point.h"
#include <vector>
#include "PrintManager.h"

namespace dnl
{

class Polyline
{
public:
	Polyline(std::string name) : m_name(name), m_points() { }
	//Polyline(const &Polyline other) : m_points(other.m_points) { }

	bool isClosed() const;

	void addPoint(dnl::Point newPoint);
	void output();
	void print(PrintManager &printMan);
	void printPolygon(PrintManager &printMan, Colour *colour = NULL);
	void getBoundingRectangle(dnl::Point &lowerLeft, dnl::Point &upperRight) const;

	// Data
	std::string m_name;
	std::vector<dnl::Point> m_points;

};

};
