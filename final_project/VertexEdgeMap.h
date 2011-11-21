
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
			if(a.m_x == b.m_x)
			{
				return a.m_y < b.m_y;
			}
			else
			{
				return a.m_x < b.m_x;
			}
	   }
	};


	// Data
	std::vector<dnl::Point> m_verticies;
	std::map<int, int> m_edges;
	std::map<dnl::Point, int, cmp_point> m_vertexHashMap;

};