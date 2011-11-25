
#pragma once

#include "Polyline.h"

namespace dnl
{

class Polygon
{
public:
	Polygon(std::string name) : m_name(name), m_outerRing(name) { }

	void setOuterRing(const dnl::Polyline &polyline);
	void addInnerRing(const dnl::Polyline &polyline);

	bool getBoundingRectangle(dnl::Point &lowerLeft, dnl::Point &upperRight);

	void print(PrintManager &printMan);

	std::string m_name;
	dnl::Polyline m_outerRing;
	std::vector<dnl::Polyline> m_innerRings;
};

}