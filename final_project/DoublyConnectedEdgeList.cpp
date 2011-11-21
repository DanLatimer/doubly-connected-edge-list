
#include "stdafx.h"
#include "common.h"
#include "DoublyConnectedEdgeList.h"
#include "utils.h"

using namespace std;
using namespace dnl;

bool DoublyConnectedEdgeList::construct()
{
	if(m_GMLFile.get() == NULL)
	{
		return false;
	}

	std::vector<dnl::Point> m_VERTEX;
	
	std::vector<Edge> m_edges;
	std::vector<int> m_firstOccuranceOfVertex;

	std::vector<int> m_firstEdgeInFace;
	std::vector<EdgeCycleEntry> m_edgeCycles;
}

void DoublyConnectedEdgeList::print(PrintManager &printMan)
{
	// TODO: implement this

	assert(0); // Unimplemented function
}