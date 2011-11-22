
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

class EdgeCycleEntry
{
public:
	EdgeCycleEntry(int vertex = -1, int next = -1) : vertex(vertex), next(next) { }
	int vertex;
	int next;
};

class DoublyConnectedEdgeList
{
public:
	// We donate the gmlFile to the DCEL when constructing it.
	DoublyConnectedEdgeList() { }

	bool construct(const VertexEdgeMap &vertexEdgeMap);

	void print(PrintManager &printMan, int printWhat);

private:
	void addEdgesForVertex(const VertexEdgeMap &vertexEdgeMap, const unsigned int vertexIndex);
	bool constructVertexCycles();
	bool constructFaceCycles();
	void findEdgesOfFace(int faceIndex, std::vector<int> &edges);

	// Data
	std::vector<dnl::Point> m_VERTEX;
	
	std::vector<Edge> m_edges;
	std::vector<int> m_firstOccuranceOfVertex;
	std::vector<int> m_firstOccuranceOfFace;

	std::vector<int> m_edgeCycleVertexIndex;
	std::vector<EdgeCycleEntry> m_edgeCycles;
	

};