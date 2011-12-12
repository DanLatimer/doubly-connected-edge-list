
#include "stdafx.h"
#include "common.h"
#include "VertexEdgeMap.h"
#include "VertexMultiEdgeMap.h"
#include "utils.h"
#include <stack>

using namespace std;
using namespace dnl;


VertexMultiEdgeMap::VertexMultiEdgeMap(const VertexEdgeMap &map)
{
	m_map = map;

	// Create all verticies
	for(unsigned int i = 0; i < m_map.m_verticies.size(); i++)
	{
		m_vertex.push_back(node(i));
	}

	// Create all edges
	for(unsigned int i = 0; i < m_map.m_edges.size(); i++)
	{
		for(unsigned int j = 0; j < m_map.m_edges[i].size(); j++)
		{
			m_vertex[i].edges.push_back(&m_vertex[m_map.m_edges[i][j]]);
		}
	}

	// Compress Edges
	{
		std::stack< std::pair<node *, node *> > edgeStack;

		// Create vertex marker flags
		// TODO: [Efficency improvement] turn into a dynamically allocated 
		//       2d array of booleans instead of vectors.
		std::vector< std::vector<bool> > m_edgesVisited;
		for(unsigned int i = 0; i < m_vertex.size(); i++)
		{
			std::vector<bool> oneDVector;
			for(unsigned int j = 0; j < m_vertex.size(); j++)
			{
				oneDVector.push_back(false);
			}
			m_edgesVisited.push_back(oneDVector);
		}

		// Find a vertex of degree != 2 to start with (not part of a multi-edge
		for(unsigned int i = 0; i < m_vertex.size(); i++)
		{
			if(m_vertex[i].edges.size() != 2 && m_vertex[i].edges.size() != 0)
			{
				// push all edges incident on starting vertex onto the stack.
				for(unsigned int j = 0; j < m_vertex[i].edges.size(); j++)
				{
					std::pair<node *, node *> edge(&m_vertex[i], m_vertex[i].edges[j]);
					edgeStack.push(edge);
				}
				break;
			}
		}

		if(edgeStack.size() == 0)
		{
			assert(0);
			return;
		}

		// NOTE: stack only holds edges that are candidates for first edge in an 
		// edge compression. Therefore they must start with degree != 2 && degree != 0

		// Traverse the graph
		while(edgeStack.size() > 0)
		{
			node *node1 = edgeStack.top().first;
			node *node2 = edgeStack.top().second;
			edgeStack.pop();

			// push all unvisited incident edges onto stack
			for(unsigned int i = 0; i < node2->edges.size(); i++)
			{
				// if edge unvisited
				if(m_edgesVisited[node2->vertexIndex][node2->edges[i]->vertexIndex] == false) 
				{
					std::pair<node *, node *> edge(node2, node2->edges[i]);
					edgeStack.push(edge);
				}
			}




		}

	}
}