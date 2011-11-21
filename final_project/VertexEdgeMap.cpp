
#include "stdafx.h"
#include "common.h"
#include "VertexEdgeMap.h"
#include "utils.h"

using namespace std;
using namespace dnl;

int VertexEdgeMap::getVertexIndex(const dnl::Point & point)
{
	map<dnl::Point, int, cmp_point>::iterator it = 
		m_vertexHashMap.find(point);

	if(it == m_vertexHashMap.end())
	{
		// Vertex not yet in map, add it.
		m_verticies.push_back(point);
		m_vertexHashMap[point] = m_verticies.size() - 1;
		return m_verticies.size() - 1;
	}
	else
	{
		return it->second;
	}
}

bool VertexEdgeMap::construct(GMLFile *gmlFile)
{
	if(gmlFile == NULL)
	{
		return false;
	}
	
	// Process GMLFile's lines
	for(unsigned int i = 0; i < gmlFile->m_lines.size(); i++)
	{
		// Process a line;
		const dnl::Polyline &currentLine = gmlFile->m_lines[i];

		int previousVertexIndex = -1;
		for(unsigned int j = 0; j < currentLine.m_points.size(); j++)
		{
			const dnl::Point &currentPoint = currentLine.m_points[j];
			
			const int vertexIndex = getVertexIndex(currentPoint);
			if(previousVertexIndex == -1)
			{
				previousVertexIndex = vertexIndex;
				continue;
			}
			
			if(previousVertexIndex == vertexIndex)
			{
				// Don't add edges from a vertex to itself
				continue;
			}

			const std::pair<int, int> edge(previousVertexIndex, vertexIndex);
			m_edges.insert(edge);
			previousVertexIndex = vertexIndex;
		}
	}
	return true;
}

void VertexEdgeMap::print(PrintManager &printMan)
{
	std::map<int, int>::iterator currentEdge = m_edges.begin();
	for(; currentEdge != m_edges.end(); currentEdge++)
	{
		// Get the two points the edge is made of
		const dnl::Point &point1 = m_verticies[currentEdge->first];
		const dnl::Point &point2 = m_verticies[currentEdge->second];

		printMan.PrintLine(point1, point2);
	}
}