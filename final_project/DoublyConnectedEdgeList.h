
#pragma once

#include "PrintManager.h"
#include "VertexEdgeMap.h"

class Edge {
public:
	Edge() : 
	  vertex1(-1), 
	  vertex2(-1), 
	  face1(-1), 
	  face2(-1), 
	  nextEdgeVertex1(-1), 
	  nextEdgeVertex2(-1) { }

	int vertex1;
	int vertex2;
	int face1;
	int face2;
	int nextEdgeVertex1;
	int nextEdgeVertex2;
};

class DoublyConnectedEdgeList
{
public:
	// We donate the gmlFile to the DCEL when constructing it.
	DoublyConnectedEdgeList(std::auto_ptr<VertexEdgeMap> vertexEdgeMap) : m_vertexEdgeMap(vertexEdgeMap) { }

	bool construct();

	void print(PrintManager &printMan);

private:
	void addEdgesForVertex(unsigned int vertexIndex);
	bool constructVertexCycles();

	// Data
	std::auto_ptr<VertexEdgeMap> m_vertexEdgeMap;
	
	std::vector<dnl::Point> m_VERTEX;
	
	std::vector<Edge> m_edges;
	std::vector<int> m_firstOccuranceOfVertex;

	std::vector<int> m_edgeCycleVertexIndex;
	std::vector<int> m_edgeCycles;
	

};