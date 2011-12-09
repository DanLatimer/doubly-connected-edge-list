
#pragma once

#include "PrintManager.h"
#include "GMLFile.h"

class VertexEdgeMap
{
public:
	VertexEdgeMap() { }

	bool construct(GMLFile *gmlFile);
	void print(PrintManager &printMan);
	int getVertexIndex(const dnl::Point & point);

private:
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
public:
	std::vector<dnl::Point> m_verticies;
	std::vector< std::vector<int> > m_edges;
	std::map<dnl::Point, int, cmp_point> m_vertexHashMap;

	double m_llX;
	double m_llY;
	double m_urX;
	double m_urY;

};