
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

	friend bool operator== (Edge &cP1, Edge &cP2)
	{
		return cP1.vertex1 == cP2.vertex1 &&
			   cP1.vertex2 == cP2.vertex2 &&
			   cP1.face1 == cP2.face1 &&
			   cP1.face2 == cP2.face2 &&
			   cP1.nextEdgeVertex1 == cP2.nextEdgeVertex1 &&
			   cP1.nextEdgeVertex2 == cP2.nextEdgeVertex2;
	}
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
	bool findEdgesOfFace(int faceIndex, std::vector< std::pair<int, bool> > &edges);
	void createFaces();
	int findNextNonDangle(
		const int theFace, 
		const int currentEdge, 
		std::map<int, bool> &edgesChecked,
		const std::vector< std::pair<int, bool> > &edges);
	void bruteForcePrintFace(PrintManager &printMan, int faceNum);
	bool isPolygonClosed(const std::vector< std::pair<int, bool> > &edges);

	// Data
	std::vector<dnl::Point> m_VERTEX;
	std::vector< std::vector<dnl::Point> > m_FACES;
	std::vector< std::vector<std::pair<int, bool> > > m_FACEEdges;
	
	std::vector<Edge> m_edges;
	std::vector<int> m_firstOccuranceOfVertex;
	std::vector<int> m_firstOccuranceOfFace;

	std::vector<int> m_edgeCycleVertexIndex;
	std::vector<EdgeCycleEntry> m_edgeCycles;
	

};