
#pragma once

#include "PrintManager.h"
#include "GMLFile.h"

class Edge {
public:
	int vertex1;
	int vertex2;
	int face1;
	int face2;
	int nextEdgeVertex1;
	int nextEdgeVertex2;
};

class EdgeCycleEntry {
public:
	int m_vertex;
	int m_nextVertex;
};

class DoublyConnectedEdgeList
{
public:
	// We donate the gmlFile to the DCEL when constructing it.
	DoublyConnectedEdgeList(std::auto_ptr<GMLFile> gmlFile) : m_GMLFile(gmlFile) { }

	bool construct();

	void print(PrintManager &printMan);

private:
	// Data
	std::auto_ptr<GMLFile> m_GMLFile;
	
	std::vector<dnl::Point> m_VERTEX;
	
	std::vector<Edge> m_edges;
	std::vector<int> m_firstOccuranceOfVertex;

	std::vector<int> m_firstEdgeInFace;
	std::vector<EdgeCycleEntry> m_edgeCycles;
	

};