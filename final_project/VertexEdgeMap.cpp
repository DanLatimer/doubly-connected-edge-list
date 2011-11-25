
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

			// Add more vectors as needed.
			for(unsigned int i = m_edges.size(); i <= vertexIndex || i <= previousVertexIndex; i++)
			{
				std::vector<int> oneDVector;
				m_edges.push_back(oneDVector);
			}

			m_edges[previousVertexIndex].push_back(vertexIndex);
			m_edges[vertexIndex].push_back(previousVertexIndex);

			previousVertexIndex = vertexIndex;
		}
	}

	// Process GMLFile's areas
	for(unsigned int i = 0; i < gmlFile->m_areas.size(); i++)
	{
		// Process a line;
		const dnl::Polygon &currentPolygon = gmlFile->m_areas[i];

		const dnl::Polyline &outerRing = currentPolygon.m_outerRing;

		int previousVertexIndex = -1;
		for(unsigned int j = 0; j < outerRing.m_points.size(); j++)
		{
			const dnl::Point &currentPoint = outerRing.m_points[j];
			
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

			// Add more vectors as needed.
			for(unsigned int i = m_edges.size(); i <= vertexIndex || i <= previousVertexIndex; i++)
			{
				std::vector<int> oneDVector;
				m_edges.push_back(oneDVector);
			}

			m_edges[previousVertexIndex].push_back(vertexIndex);
			m_edges[vertexIndex].push_back(previousVertexIndex);

			previousVertexIndex = vertexIndex;
		}
	}
	return true;
}

void VertexEdgeMap::print(PrintManager &printMan)
{
	for(unsigned int i = 0; i < m_verticies.size(); i++)
	{
		const dnl::Point &point1 = m_verticies[i];
		for(unsigned int j = 0; j < m_edges[i].size(); j++)
		{
			if(i > m_edges[i][j])
			{
				// We've already printed this edge
				continue;
			}

			// Get the two points the edge is made of
			const dnl::Point &point2 = m_verticies[m_edges[i][j]];

			printMan.PrintLine(point1, point2);
		}
	}
}