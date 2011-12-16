#include "stdafx.h"

#include "common.h"
#include "KDTreeNode.h"
#include "Point.h"

using namespace std;

KDTreeNode::KDTreeNode(int vertexIndex) 
	: m_vertexIndex(vertexIndex), m_left(NULL), m_right(NULL), m_discriminator(Unspecified)
{

}

KDTreeNode::KDTreeNode() 
	: m_vertexIndex(-1), m_left(NULL), m_right(NULL), m_discriminator(Unspecified)
{

}

bool KDTreeNode::isEqual(const KDTreeNode &other, const std::vector<dnl::Point> &verticies) const
{
	const dnl::Point &myPoint = verticies[m_vertexIndex];
	const dnl::Point &otherPoint = verticies[other.m_vertexIndex];

	return (myPoint.m_x == otherPoint.m_x) && 
		   (myPoint.m_y == otherPoint.m_y);
}

int KDTreeNode::compare(const double x, const double y, DType discriminator, const std::vector<dnl::Point> &verticies) const
{
	const dnl::Point &myPoint = verticies[m_vertexIndex];

	switch(discriminator)
	{
	case Latitude:
		return myPoint.m_x - x;
		
	case Longitude:
		return myPoint.m_y - y;

	case Unspecified:
	default:
		assert(0);
		return 0;
	}
}

int KDTreeNode::compare(const KDTreeNode &other, DType discriminator, const std::vector<dnl::Point> &verticies) const
{
	const dnl::Point &otherPoint = verticies[other.m_vertexIndex];

	return compare(otherPoint.m_x, otherPoint.m_y, discriminator, verticies);
}

DType KDTreeNode::getNextDiscriminator() const
{
	switch(m_discriminator)
	{
	case Latitude:
		return Longitude;

	case Longitude:
		return Latitude;

	case Unspecified:
	default:
		// Invalid state, fail.
		assert(0);
		return Unspecified;
	}
}
