
#pragma once

#include "PrintManager.h"
#include "GMLFile.h"

class VertexEdgeMap
{
public:
	VertexEdgeMap() { }

	bool construct(GMLFile *gmlFile);
	void print(PrintManager &printMan);

private:
	int getVertexIndex(const dnl::Point & point);
	struct cmp_point
	{
	   bool operator()(const dnl::Point &a, const dnl::Point &b) const
	   {
		   // NOT a good solution
		   return (a.m_x + a.m_y) < (b.m_x + b.m_y);
	   }
	};


	// Data
	std::vector<dnl::Point> m_verticies;
	std::map<int, int> m_edges;
	std::map<dnl::Point, int, cmp_point> m_vertexHashMap;

};