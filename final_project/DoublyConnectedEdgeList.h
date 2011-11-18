
#pragma once

#include "PrintManager.h"
#include "GMLFile.h"


class DoublyConnectedEdgeList
{
public:
	// We donate the gmlFile to the DCEL when constructing it.
	DoublyConnectedEdgeList(std::auto_ptr<GMLFile> gmlFile) : m_GMLFile(gmlFile) { }

	void print(PrintManager &printMan);

private:
	// Data
	std::auto_ptr<GMLFile> m_GMLFile;

};