
#include "stdafx.h"
#include "common.h"
#include "GMLFile.h"

#include <iostream>

#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

using namespace rapidxml;
using namespace std;

void GMLFile::parse(const string &filename)
{

	ifstream fin("vermont_roads.gml");
	if (fin.fail()) return;

	fin.seekg(0, ios::end);
	size_t length = fin.tellg();
	fin.seekg(0, ios::beg);
	char* buffer = new char[length + 1];
	fin.read(buffer, length);
	buffer[length] = '\0';

	fin.close();

	xml_document<> doc;
	doc.parse<0>(buffer);

	delete [] buffer;

}