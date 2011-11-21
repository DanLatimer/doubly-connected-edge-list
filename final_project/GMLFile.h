
#pragma once

#include "rapidxml.hpp"
#include "Polyline.h"
#include "Polygon.h"

class GMLFile
{
public:
	GMLFile() : m_doc() { }

	bool parse(const std::string &filename);

	void print(PrintManager &printMan);

	// Boundary
	double m_llX;
	double m_llY;
	double m_urX;
	double m_urY;

private:
	bool parseFeatures(rapidxml::xml_node<> *allFeatures);
	bool parseFeature(rapidxml::xml_node<> *currentFeature);
	bool parseRing(rapidxml::xml_node<> *ringNode, dnl::Polyline &ring);

	bool createBoundingBox(dnl::Point &llBox, dnl::Point &urBox);


	// Data
	rapidxml::xml_document<> m_doc;

public:
	std::vector<dnl::Polyline> m_lines;
	std::vector<dnl::Polygon> m_areas;
};