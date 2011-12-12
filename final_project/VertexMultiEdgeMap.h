
#pragma once

#include "VertexEdgeMap.h"

class node
{
public:
	node(int vertexIndex) : vertexIndex(vertexIndex) { }
	unsigned int degree()
	{
		return edges.size();
	}	

	int vertexIndex;
	std::vector<node *> edges;
};

class VertexMultiEdgeMap
{
public:
	VertexMultiEdgeMap(const VertexEdgeMap &map);

private:
	VertexEdgeMap m_map;
	std::vector<node> m_vertex;
	std::vector<std::vector<int> > m_multiEdgeEdges;
};