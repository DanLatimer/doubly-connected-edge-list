
#pragma once

#include <autoserial/autoserial.h>

#include "PrintManager.h"
#include "VertexEdgeMap.h"
#include "KDTree.h"

class Edge : public autoserial::ISerializable  
{
public:
	Edge() : 
	  vertex1(-1), 
	  vertex2(-1), 
	  face1(-1), 
	  face2(-1), 
	  nextEdgeVertex1(-1), 
	  nextEdgeVertex2(-1) { }

	AS_CLASSDEF(Edge)                     
    AS_MEMBERS                                      
		AS_PUBLICITEM(int, vertex1)
        AS_PUBLICITEM(int, vertex2)
        AS_PUBLICITEM(int, face1)
        AS_PUBLICITEM(int, face2)
        AS_PUBLICITEM(int, nextEdgeVertex1)
        AS_PUBLICITEM(int, nextEdgeVertex2)
    AS_CLASSEND;  

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

class DirectedEdge : public autoserial::ISerializable 
{
public:
	DirectedEdge(int edge = -1, bool direction = false) : edge(edge), direction(direction) { }
	
	AS_CLASSDEF(DirectedEdge)                     
    AS_MEMBERS                                      
		AS_PUBLICITEM(int, edge)
        AS_PUBLICITEM(bool, direction)
    AS_CLASSEND;  
};

class EdgeCycleEntry : public autoserial::ISerializable 
{
public:
	EdgeCycleEntry(int vertex = -1, int next = -1) : vertex(vertex), next(next) { }
	AS_CLASSDEF(EdgeCycleEntry)                     
    AS_MEMBERS                                      
		AS_PUBLICITEM(int, vertex)
        AS_PUBLICITEM(int, next)
    AS_CLASSEND;  
};

class DoublyConnectedEdgeList : public autoserial::ISerializable 
{
public:
	DoublyConnectedEdgeList() : m_KDTree(NULL) { }

	bool construct(const VertexEdgeMap &vertexEdgeMap);
	void print(PrintManager &printMan, int printWhat, int index = 0);

	bool findEdgesOfFace(int faceIndex, std::vector<DirectedEdge> &edges);
	bool findEdgesOfVertex(int vertexIndex, std::vector<DirectedEdge> &edges);

	void constructKDTree();

	// Cached variable
	KDTree *m_KDTree;

private:
	void addEdgesForVertex(const VertexEdgeMap &vertexEdgeMap, const unsigned int vertexIndex);
	bool constructVertexCycles();
	bool constructFaceCycles();
	void createFaces();
	int findNextNonDangle(
		const int theFace, 
		int currentEdge, 
		std::map<int, bool> &edgesChecked,
		const std::vector<DirectedEdge> &edges);
	void bruteForcePrintFace(PrintManager &printMan, int faceNum);
	bool isPolygonClosed(const std::vector<DirectedEdge> &edges);

	// Data
	AS_CLASSDEF(DoublyConnectedEdgeList)                     // Declare class name
    AS_MEMBERS                                               // Start list of class members
        AS_PUBLICITEM(std::vector<dnl::Point>, m_VERTEX)
        
		AS_PUBLICITEM(std::vector<Edge>, m_edges)
        AS_PUBLICITEM(std::vector<int>, m_firstOccuranceOfVertex)
		AS_PUBLICITEM(std::vector<int>, m_firstOccuranceOfFace)
        
		AS_PRIVATEITEM(std::vector<int>, m_edgeCycleVertexIndex)
        AS_PRIVATEITEM(std::vector<EdgeCycleEntry>, m_edgeCycles)
    
        AS_PUBLICITEM(double, m_llX)
        AS_PUBLICITEM(double, m_llY)
        AS_PUBLICITEM(double, m_urX)
        AS_PUBLICITEM(double, m_urY)

	AS_CLASSEND;  

private:
	// CACHE VARIABLES
	// Constructed when needed and saved only in non-persistent memory.
	std::vector< std::vector<dnl::Point> > m_FACES;
	std::vector< std::vector<DirectedEdge> > m_FACEEdges;

};