
#pragma once 

#include "common.h"
#include "Point.h"

enum Direction
{
	Left = 0,
	Right = 1
};

enum DType 
{
	Unspecified = 0,
	Latitude = 1,
	Longitude = 2,
	Name = 3
};

class KDTreeNode
{
public:
	KDTreeNode(); // must set m_vertexIndex after instantiation
	KDTreeNode(int vertexIndex);

	bool isEqual(const KDTreeNode &other, const std::vector<dnl::Point> &verticies) const;
	int compare(const KDTreeNode &other, DType discriminator, const std::vector<dnl::Point> &verticies) const;
	int compare(const double x, const double y, DType discriminator, const std::vector<dnl::Point> &verticies) const;
	DType getNextDiscriminator() const;

public:
	int m_vertexIndex;

	KDTreeNode *m_left;			
	KDTreeNode *m_right;
	DType m_discriminator;
	
};
