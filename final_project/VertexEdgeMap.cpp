
#include "stdafx.h"
#include "common.h"
#include "VertexEdgeMap.h"
#include "utils.h"
#include "math.h"

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

	m_llX = gmlFile->m_llX;
	m_llY = gmlFile->m_llY;
	m_urX = gmlFile->m_urX;
	m_urY = gmlFile->m_urY;

	const int fivePercentLines = ceil((double)gmlFile->m_lines.size() / 20.0);
	std::wstring message = L"Step 2/3: Constructing Vertex-EdgeMap from ";	
	message += utils::StringToWString(utils::parseLong(gmlFile->m_lines.size()));
	message += L" lines [0/100] and ";
	message += utils::StringToWString(utils::parseLong(gmlFile->m_areas.size()));
	message += L" areas [0/100].";

	utils::getInstance()->setTextOnStatusBar(2, message.c_str());

	
	// Process GMLFile's lines
	for(unsigned int i = 0; i < gmlFile->m_lines.size(); i++)
	{
		if(i % fivePercentLines == 0)
		{
			const int percentComplete = ceil(((double)i)*100.0/((double)gmlFile->m_lines.size()));
			std::wstring message = L"Step 2/3: Constructing Vertex-EdgeMap from ";	
			message += utils::StringToWString(utils::parseLong(gmlFile->m_lines.size()));
			message += L" lines [";
			message += utils::StringToWString(utils::parseLong(percentComplete));
			message += L"/100] and ";
			message += utils::StringToWString(utils::parseLong(gmlFile->m_areas.size()));
			message += L" areas [0/100].";
			utils::getInstance()->setTextOnStatusBar(2, message.c_str());
		}

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

	const int fivePercentAreas = ceil((double)gmlFile->m_areas.size() / 20.0);
	std::wstring message2 = L"Step 2/3: Constructing Vertex-EdgeMap from ";	
	message2 += utils::StringToWString(utils::parseLong(gmlFile->m_lines.size()));
	message2 += L" lines [100/100] and ";
	message2 += utils::StringToWString(utils::parseLong(gmlFile->m_areas.size()));
	message2 += L" areas [0/100].";

	utils::getInstance()->setTextOnStatusBar(2, message2.c_str());

	// Process GMLFile's areas
	for(unsigned int i = 0; i < gmlFile->m_areas.size(); i++)
	{
		if(i % fivePercentAreas == 0)
		{
			const int percentComplete = ceil(((double)i)*100.0/((double)gmlFile->m_areas.size()));
			std::wstring message = L"Step 2/3: Constructing Vertex-EdgeMap from ";	
			message += utils::StringToWString(utils::parseLong(gmlFile->m_lines.size()));
			message += L" lines [100/100] and ";
			message += utils::StringToWString(utils::parseLong(gmlFile->m_areas.size()));
			message += L" areas [";
			message += utils::StringToWString(utils::parseLong(percentComplete));
			message += L"/100].";
			utils::getInstance()->setTextOnStatusBar(2, message.c_str());
		}

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