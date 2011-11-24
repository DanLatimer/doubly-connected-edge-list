
#include "stdafx.h"
#include "common.h"
#include "DoublyConnectedEdgeList.h"
#include "utils.h"
#include <algorithm>
#include <cmath>

using namespace std;
using namespace dnl;

// http://softsurfer.com/Archive/algorithm_0109/algorithm_0109.htm#isLeft()
// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
float isLeft( dnl::Point P0, dnl::Point P1, dnl::Point P2 )
{
    return (P1.m_x - P0.m_x)*(P2.m_y - P0.m_y) - (P2.m_x - P0.m_x)*(P1.m_y - P0.m_y);
}

double get_angle(const dnl::Point &point1, const dnl::Point &point2)	
{
	double x1 = point1.m_x;
	double y1 = point1.m_y;
	double x2 = point2.m_x;
	double y2 = point2.m_y;

	if(x1==x2 && y1==y2)
	{
		assert(0);
		return(-1);
	}

	double opposite;
    double adjacent;
    double angle;

    //calculate vector differences
    opposite=y1-y2;
    adjacent=x1-x2;
 
    //trig function to calculate angle
    if(adjacent==0) // to catch vertical co-ord to prevent division by 0
    {
		return (opposite >= 0) ? 0 : 180;
    }
    else 
    {
		//angle = opposite/adjacent;
        angle=(atan(opposite/adjacent))*180/PI;
        //the angle calculated will range from +90 degrees to -90 degrees
        //so the angle needs to be adjusted if point x1 is less or greater then x2
        if(x1>=x2)
        {
            angle=90-angle;
        }
        else
        {
            angle=270-angle;
        }
    } 
    return angle;
}


bool DoublyConnectedEdgeList::constructVertexCycles()
{
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
		int t = m_edgeCycleVertexIndex[i];
		assert(t != -1);
		int r = m_edgeCycles[t].vertex;

		if(r > i)
		{
			m_edges.push_back(Edge());
			assert(m_edges.size() - 1 >= numEdges);

			m_edges[numEdges].vertex1 = i;
			m_edges[numEdges].vertex2 = r;

			m_firstOccuranceOfVertex[i] = numEdges;
			currentVertexCycle = numEdges;
			v1Or2 = 1;
			m_edges[numEdges].nextEdgeVertex2 = lastPosition[r];
			lastPosition[r] = numEdges;
			numEdges++;
		}
		else
		{
			assert(B[r] != -1);
			m_firstOccuranceOfVertex[i] = B[r];
			currentVertexCycle = B[r];
			v1Or2 = 2;
		}

		// Complete the construction of the vertex cycle for vi
		while(m_edgeCycles[t].next != m_edgeCycleVertexIndex[i])
		{
			t = m_edgeCycles[t].next;
			r = m_edgeCycles[t].vertex;
			if(r > i)
			{
				// Enter into vertex cycle
				m_edges.push_back(Edge());
				assert(m_edges.size() - 1 >= numEdges);

				m_edges[numEdges].vertex1 = i;
				m_edges[numEdges].vertex2 = r;
				m_edges[numEdges].nextEdgeVertex2 = lastPosition[r];
				lastPosition[r] = numEdges;
				if(v1Or2 == 1)
				{
					m_edges[currentVertexCycle].nextEdgeVertex1 = numEdges;
				}
				else
				{
					m_edges[currentVertexCycle].nextEdgeVertex2 = numEdges;
				}
				currentVertexCycle = numEdges;
				v1Or2 = 1;
				numEdges++;
			}
			else
			{
				// r < i so link this edge into the vertex cycle
				if(v1Or2 == 1)
				{
					m_edges[currentVertexCycle].nextEdgeVertex1 = B[r];
				}
				else
				{
					m_edges[currentVertexCycle].nextEdgeVertex2 = B[r];
				}
				currentVertexCycle = B[r];
				v1Or2 = 2;
			}
		}
	
		// Close Vertex Cycle
		if(v1Or2 == 1)
		{
			m_edges[currentVertexCycle].nextEdgeVertex1 = m_firstOccuranceOfVertex[i];
		}
		else
		{
			m_edges[currentVertexCycle].nextEdgeVertex2 = m_firstOccuranceOfVertex[i];
		}
	}

	return true;
}

bool compareAnglePairs (
	const std::pair<double, int> &left, 
	const std::pair<double, int> &right) 
{ 
	return left.first < right.first; 
}

void DoublyConnectedEdgeList::addEdgesForVertex(const VertexEdgeMap &vertexEdgeMap, const unsigned int vertexIndex)
{
	m_VERTEX[vertexIndex];

	// Get all edges incident on the vertex;

	const std::vector<int> &edgesOnVertex = 
		vertexEdgeMap.m_edges[vertexIndex];

	if(edgesOnVertex.size() < 1)
	{
		ReportError("Vertex without edges... might indicate an error");
		return;
	}

	const dnl::Point &vertex = m_VERTEX[vertexIndex];

	std::vector< std::pair<double, int> > angles;
	for(unsigned int i = 0; i < edgesOnVertex.size(); i++)
	{
		const dnl::Point &vertex2 = m_VERTEX[edgesOnVertex[i]];
		const double angle = get_angle(vertex, vertex2);

		angles.push_back(std::pair<double, int> (angle, edgesOnVertex[i]));
	}

	std::sort(angles.begin(), angles.end(), compareAnglePairs);
	
	// Add Entry in m_edgeCycleVertexIndex (aka H)
	const int firstEntryForVertex = m_edgeCycles.size();
	m_edgeCycleVertexIndex.push_back(firstEntryForVertex);

	// Add entries in m_edgeCycles
	for(unsigned int i = 0; i < angles.size(); i++)
	{
		EdgeCycleEntry entry(angles[i].second, m_edgeCycles.size()+1);
		if(i == angles.size() - 1)
		{
			entry.next = firstEntryForVertex;
		}
		m_edgeCycles.push_back(entry);
	}

	/*
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
	*/
}

bool DoublyConnectedEdgeList::constructFaceCycles()
{
	int currentEdge = 0; //a
	int v1Or2 = -1; //u

	int c;
	int s = 0;
	int facesSet = 0; //k
	int firstEdge = 0; //a0
	while(facesSet <= 2*(m_edges.size()-1))
	{
		if(m_edges[currentEdge].face1 != -1 && 
		   m_edges[currentEdge].face2 != -1)
		{
			currentEdge++;
		}
		else
		{
			// First time for this face
			v1Or2 = (m_edges[currentEdge].face1 == -1) ? 1 : 2;
			if(v1Or2 == 1)
			{
				m_edges[currentEdge].face1 = s;
				c = m_edges[currentEdge].vertex1;
				m_firstOccuranceOfFace.push_back(-1);
				m_firstOccuranceOfFace[s] = currentEdge;
				firstEdge = currentEdge;
				facesSet++;
				currentEdge = m_edges[currentEdge].nextEdgeVertex1;
			}
			else
			{
				m_edges[currentEdge].face2 = s;
				c = m_edges[currentEdge].vertex2;
				m_firstOccuranceOfFace.push_back(-1);
				m_firstOccuranceOfFace[s] = currentEdge;
				firstEdge = currentEdge;
				facesSet++;
				currentEdge = m_edges[currentEdge].nextEdgeVertex2;
			}
			while(currentEdge != firstEdge)
			{
				v1Or2 = (m_edges[currentEdge].vertex1 == c) ? 2 : 1;
				if(v1Or2 == 1)
				{
					m_edges[currentEdge].face1 = s;
					c = m_edges[currentEdge].vertex1;
					facesSet++;
					currentEdge = m_edges[currentEdge].nextEdgeVertex1;
				}
				else
				{
					m_edges[currentEdge].face2 = s;
					c = m_edges[currentEdge].vertex2;
					facesSet++;
					currentEdge = m_edges[currentEdge].nextEdgeVertex2;
				}
			}
			s++;
		}
	}
	return true;
}

bool DoublyConnectedEdgeList::construct(const VertexEdgeMap &vertexEdgeMap)
{
	m_VERTEX = vertexEdgeMap.m_verticies;

	for(unsigned int i = 0; i < m_VERTEX.size(); i++)
	{
		m_firstOccuranceOfVertex.push_back(-1);
	}

	for(unsigned int i = 0; i < m_VERTEX.size(); i++)
	{
		addEdgesForVertex(vertexEdgeMap, i);
	}


	bool success = constructVertexCycles();
	if(success == false)
	{
		ReportError("Unable to construct DCEL's Vertex Cycles");
		return false;
	}

	// Check that all things that should be populated by this point, are populated
	for(unsigned int i = 0; i < m_edges.size(); i++)
	{
		assert(m_edges[i].vertex1 != -1);
		assert(m_edges[i].vertex2 != -1);
		assert(m_edges[i].nextEdgeVertex1 != -1);
		assert(m_edges[i].nextEdgeVertex2 != -1);
	}

	success = constructFaceCycles();
	if(success == false)
	{
		ReportError("Unable to construct DCEL's Face Cycles");
		return false;
	}

	// Check that all things that should be populated by this point, are populated
	for(unsigned int i = 0; i < m_edges.size(); i++)
	{
		assert(m_edges[i].vertex1 != -1);
		assert(m_edges[i].vertex2 != -1);
		assert(m_edges[i].face1 != -1);
		assert(m_edges[i].face2 != -1);
		assert(m_edges[i].nextEdgeVertex1 != -1);
		assert(m_edges[i].nextEdgeVertex2 != -1);
	}

	createFaces();

	/*std::vector<dnl::Point> m_VERTEX;
	
	std::vector<Edge> m_edges;
	std::vector<int> m_firstOccuranceOfVertex;

	std::vector<int> m_firstEdgeInFace;
	std::vector<EdgeCycleEntry> m_edgeCycles;*/
	return true;
}

bool DoublyConnectedEdgeList::findEdgesOfFace(int theFace, std::vector< std::pair<int, bool> > &edges)
{
	int currentEdge = m_firstOccuranceOfFace[theFace]; //a
	int firstEdge = currentEdge; //a0

	bool forward = (m_edges[currentEdge].face1 != theFace);
	edges.push_back(std::pair<int, bool>(currentEdge, forward));

	if(!forward)
	{
		// Negative Edge?
		currentEdge = m_edges[currentEdge].nextEdgeVertex1;
	}
	else
	{
		// Positive Edge?
		currentEdge = m_edges[currentEdge].nextEdgeVertex2;
	}

	while(currentEdge != firstEdge)
	{
		// Never enter an edge that has the same face on both sides, look for alternative
		if(m_edges[currentEdge].face1 == m_edges[currentEdge].face2)
		{
			return false;
		}

		forward = (m_edges[currentEdge].face1 != theFace);
		edges.push_back(std::pair<int, bool>(currentEdge, forward));

		if(!forward)
		{
			// Negative Edge?
			currentEdge = m_edges[currentEdge].nextEdgeVertex1;
		}
		else
		{
			// Positive Edge?
			currentEdge = m_edges[currentEdge].nextEdgeVertex2;
		}


		// Follow the lines
		/*
		if(currentEdge == (edges.end()-1)->first)
		{
			forward = !forward;	
		}
		else
		{
			if(m_edges[currentEdge].face1 != m_edges[currentEdge].face2)
			{
				forward = (m_edges[currentEdge].face1 != faceIndex);
			}
			edges.push_back(std::pair<int, bool>(currentEdge, forward));
		}

		if(!forward)
		{
			// Negative Edge?
			currentEdge = m_edges[currentEdge].nextEdgeVertex1;
		}
		else
		{
			// Positive Edge?
			currentEdge = m_edges[currentEdge].nextEdgeVertex2;
		}
		*/
	}
	return true;
}

void DoublyConnectedEdgeList::createFaces()
{
	for(unsigned int i = 0; i < m_firstOccuranceOfFace.size(); i++)
	{
		assert(m_firstOccuranceOfFace[i] != -1);


		std::vector<dnl::Point> points;
		std::vector<std::pair<int, bool> > edgeIndicies;
		bool success = findEdgesOfFace(i, edgeIndicies);
		if(!success)
		{
			m_FACES.push_back(points);
			continue;
		}

		for(unsigned int j = 0; j < edgeIndicies.size(); j++)
		{
			if(j == 0)
			{
				if(edgeIndicies[j].second)
				{
					points.push_back(m_VERTEX[m_edges[edgeIndicies[j].first].vertex1]);
				}
				else
				{
					points.push_back(m_VERTEX[m_edges[edgeIndicies[j].first].vertex2]);
				}
			}

			if(edgeIndicies[j].second)
			{
				points.push_back(m_VERTEX[m_edges[edgeIndicies[j].first].vertex2]);
			}
			else
			{
				points.push_back(m_VERTEX[m_edges[edgeIndicies[j].first].vertex1]);
			}
		}

		m_FACES.push_back(points);
	}
}

void DoublyConnectedEdgeList::print(PrintManager &printMan, int printWhat)
{
	switch(printWhat)
	{
	case 1:
	{
		// Print Edges
		for(unsigned int i = 0; i < m_edges.size(); i++)
		{
			dnl::Point vertex1 = m_VERTEX[m_edges[i].vertex1];
			dnl::Point vertex2 = m_VERTEX[m_edges[i].vertex2];

			printMan.PrintLine(vertex1, vertex2, &printMan.m_solidBlack); 
		}
		break;
	}
	case 2:
	{
		// Print Faces
		for(unsigned int i = 0; i < m_FACES.size(); i++)
		{
			printMan.PrintPolygon(m_FACES[i], &printMan.getRandomColour(255));
		}
		break;
	}
	default:
		{
			ReportError(printWhat + " is an invalid choice of what part to print for a DCEL.");
			break;
		}
	}
}