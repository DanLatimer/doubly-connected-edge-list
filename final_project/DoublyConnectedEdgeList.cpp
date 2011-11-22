
#include "stdafx.h"
#include "common.h"
#include "DoublyConnectedEdgeList.h"
#include "utils.h"
#include <algorithm>

using namespace std;
using namespace dnl;

bool DoublyConnectedEdgeList::constructVertexCycles()
{
	/*
	int numEdges = 0;
	int currentVertexCycle = -1;
	int v1Or2 = -1;
	std::vector<int> lastPosition;
	for(unsigned int i = 0; i < m_VERTEX.size(); i++)
	{
		lastPosition.push_back(-1);
	}
	std::vector<int> B;
	for(unsigned int i = 0; i < m_VERTEX.size(); i++)
	{
		B.push_back(-1);
	}

	// Start looking at verticies
	for(unsigned int i = 0; i < m_VERTEX.size(); i++)
	{
		int last = lastPosition[i];
		while(last != -1)
		{
			// Fetch the location of all edges (vi, vr) with i < r by scanning
			// The temporary list of vi and storing them in array B.
			int p = m_edges[last].vertex1;
			B[p] = last;
			last = m_edges[last].nextEdgeVertex2;
		}

		// Process the first member (vi, vr) of the input edge list of vi.
		// Edge only entered if r > i
		int t = firstEdgeInFace[i];
		assert(t != -1);
		int r = m_VERTEX[t];

		if(r > i)
		{
			//Edges[numEdges].
		}


	}

*/
	return true;
}

bool compareAnglePairs (
	const std::pair<double, int> &left, 
	const std::pair<double, int> &right) 
{ 
	return left.first < right.first; 
}

void DoublyConnectedEdgeList::addEdgesForVertex(const unsigned int vertexIndex)
{
	m_VERTEX[vertexIndex];

	// Get all edges incident on the vertex;

	const std::vector<int> &edgesOnVertex = 
		m_vertexEdgeMap->m_edges[vertexIndex];

	if(edgesOnVertex.size() < 1)
	{
		ReportError("Vertex without edges... might indicate an error");
		return;
	}

	std::vector< std::pair<double, int> > angles;
	for(unsigned int i = 0; i < edgesOnVertex.size(); i++)
	{
		double angle = -1;
		// TODO: compute angle
		assert(0);

		angles.push_back(std::pair<double, int> (angle, edgesOnVertex[i]));
	}

	std::sort(angles.begin(), angles.end(), compareAnglePairs);
	
	// Add Entry in m_edgeCycleVertexIndex (aka H)
	const int firstEntryForVertex = m_edgeCycles.size();
	m_edgeCycleVertexIndex.push_back(firstEntryForVertex);

	// Add entries in m_edgeCycles
	for(unsigned int i = 0; i < angles.size(); i++)
	{
		m_edgeCycles.push_back(angles[i].second);
	}

	// Add entry to loop back to original vertex;
	m_edgeCycles.push_back(firstEntryForVertex);

	for(unsigned int i = 0; i < edgesOnVertex.size(); i++)
	{
		if(edgesOnVertex[i] > vertexIndex)
		{
			Edge current;
			current.vertex1 = vertexIndex;
			current.vertex2 = edgesOnVertex[i];

			m_edges.push_back(current);
		}
	}
}

bool DoublyConnectedEdgeList::construct()
{
	if(m_vertexEdgeMap.get() == NULL)
	{
		return false;
	}

	m_VERTEX = m_vertexEdgeMap->m_verticies;

	for(unsigned int i = 0; i < m_VERTEX.size(); i++)
	{
		addEdgesForVertex(i);
	}
	
	/*std::map<int, int>::iterator iter;
	for(iter = m_vertexEdgeMap->m_edges.begin(); iter < m_vertexEdgeMap->m_edges.end(); iter++)
	{
		Edge current;
		current.vertex1 = iter->first;
		current.vertex2 = iter->second;

		m_edges.push_back(current);
	}*/

	bool success = constructVertexCycles();
	if(success == false)
	{
		ReportError("Unable to construct DCEL's Vertex Cycles");
		return false;
	}
	//constructFaceCycles();

	/*std::vector<dnl::Point> m_VERTEX;
	
	std::vector<Edge> m_edges;
	std::vector<int> m_firstOccuranceOfVertex;

	std::vector<int> m_firstEdgeInFace;
	std::vector<EdgeCycleEntry> m_edgeCycles;*/
	return true;
}

void DoublyConnectedEdgeList::print(PrintManager &printMan)
{
	// TODO: implement this

	assert(0); // Unimplemented function
}