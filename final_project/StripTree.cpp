
#include "stdafx.h"
#include "common.h"
#include "Polyline.h"
#include "Strip.h"
#include "utils.h"
#include "StripTree.h"

using namespace dnl;

Strip * createStrip(const dnl::Polyline & polyline, int pointL, int pointR, int & midPoint)
{
	Point begin = polyline.m_points[pointL];
	Point end = polyline.m_points[pointR];

	int indexLeft = pointL;
	int indexRight = pointL;
	double widthLeft = 0;
	double widthRight = 0;
	// Only look at interior nodes within the strip
	for(int i = pointL + 1; i < pointR; i++)
	{
		double distance = utils::distanceOfPointFromLine(begin, end, polyline.m_points[i]);
		bool toTheRight = utils::pointToRightOfLine(begin, end, polyline.m_points[i]);
		
		if(toTheRight)
		{
			if(distance > widthRight)
			{
				widthRight = distance;
				indexRight = i;
			}
		}
		else
		{
			if(distance > widthLeft)
			{
				widthLeft = distance;
				indexLeft = i;
			}
		}
	}

	// Choose midpoint (The closest one to pointR)
	midPoint = indexLeft > indexRight ? indexLeft : indexRight;

	return new Strip(begin, end, widthLeft, widthRight);
}

StripTree::StripTree(const dnl::Polyline polyline, const double wStar) : m_polyline(polyline), m_root(NULL)
{
	m_root = createTree(m_polyline, 0, m_polyline.m_points.size()-1, wStar);
}

Strip * StripTree::createTree(const dnl::Polyline & polyline, int pointL, int pointR, double wStar)
{
	const int numberNodes = pointR - pointL;
	/*if(numberNodes < 2)
	{
		// done.
		return NULL;
	}*/

	int midPoint;
	Strip *currentStrip = createStrip(m_polyline, pointL, pointR, midPoint);
	if(currentStrip->getWidth() < wStar)
	{
		currentStrip->m_left = currentStrip->m_right = NULL;
		return currentStrip;
	}

	currentStrip->m_left = createTree(m_polyline, pointL, midPoint, wStar);
	currentStrip->m_right = createTree(m_polyline, midPoint, pointR, wStar);

	return currentStrip;
}

int StripTree::getSize(Strip *node)
{
	if(node == NULL)
	{
		return 0;
	}

	return getSize(node->m_left) + getSize(node->m_right) + 1;
}

int StripTree::getSize()
{
	return getSize(m_root);
}

int StripTree::getHeight(Strip *node)
{
	if(node == NULL)
	{
		return 0;
	}

	int heightLeftSubtree = getHeight(node->m_left);
	int heightRightSubtree = getHeight(node->m_right);

	return (heightLeftSubtree > heightRightSubtree) ? heightLeftSubtree + 1 : heightRightSubtree + 1;
}

int StripTree::getHeight()
{
	return getHeight(m_root);
}

Strip * StripTree::pointWithinThresholdOfPolyline(
	Strip *node, 
	const Point & point, 
	const double threshold)
{
	if(node == NULL)
	{
		return NULL;
	}

	if(threshold <= node->lowerBound(point))
	{
		return node;
	}
	if(threshold >= node->upperBound(point))
	{
		return NULL;
	}

	Strip *pointWithinLeft = 
		pointWithinThresholdOfPolyline(node->m_left, point, threshold);

	if(pointWithinLeft != NULL)
	{
		return pointWithinLeft;
	}

	Strip *pointWithinRight = 
		pointWithinThresholdOfPolyline(node->m_right, point, threshold);

	return pointWithinRight;
}

Strip * StripTree::pointWithinThresholdOfPolyline(
	const Point & point, 
	const double threshold)
{
	return pointWithinThresholdOfPolyline(m_root, point, threshold);
}

bool isInside(Strip *S1, Strip *S2)
{
	Location point1Location = S1->isInsideStrip(S2->m_stripPoints[0]);
	Location point2Location = S1->isInsideStrip(S2->m_stripPoints[1]);
	Location point3Location = S1->isInsideStrip(S2->m_stripPoints[2]);
	Location point4Location = S1->isInsideStrip(S2->m_stripPoints[3]);

	// Check for inside
	const bool s2InsideS1 = point1Location == CENTRE_MIDDLE &&
							point2Location == CENTRE_MIDDLE &&
							point3Location == CENTRE_MIDDLE &&
							point4Location == CENTRE_MIDDLE;

	return s2InsideS1;
}

bool checkForClearIntersection(Strip *S1, Strip *S2)
{
	Location p1 = S1->isInsideStrip(S2->m_stripPoints[0]);
	Location p2 = S1->isInsideStrip(S2->m_stripPoints[1]);
	Location p3 = S1->isInsideStrip(S2->m_stripPoints[2]);
	Location p4 = S1->isInsideStrip(S2->m_stripPoints[3]);

	const bool horizontalClearIntersection = 
	   ((p1 == LEFT_MIDDLE) && (p2 == LEFT_MIDDLE) && 
		(p3 == RIGHT_MIDDLE) && (p4 == RIGHT_MIDDLE)) ||
	   ((p3 == LEFT_MIDDLE) && (p4 == LEFT_MIDDLE) && 
		(p1 == RIGHT_MIDDLE) && (p2 == RIGHT_MIDDLE)); 

	const bool verticalClearIntersection = 
	   ((p1 == CENTRE_TOP) && (p2 == CENTRE_TOP) && 
		(p3 == CENTRE_BOTTOM) && (p4 == CENTRE_BOTTOM)) ||
	   ((p3 == CENTRE_TOP) && (p4 == CENTRE_TOP) && 
		(p1 == CENTRE_BOTTOM) && (p2 == CENTRE_BOTTOM));

	return horizontalClearIntersection || verticalClearIntersection;
}

bool isLeft(Location loc)
{
	return loc == LEFT_BOTTOM || loc == LEFT_MIDDLE || loc == LEFT_TOP;
}
bool isRight(Location loc)
{
	return loc == RIGHT_BOTTOM || loc == RIGHT_MIDDLE || loc == RIGHT_TOP;
}
bool isTop(Location loc)
{
	return loc == LEFT_TOP || loc == CENTRE_TOP || loc == RIGHT_TOP; 
}
bool isBottom(Location loc)
{
	return loc == LEFT_BOTTOM || loc == CENTRE_BOTTOM || loc == RIGHT_BOTTOM;
}

bool checkForNoIntersection(Strip *S1, Strip *S2)
{
	Location p1 = S1->isInsideStrip(S2->m_stripPoints[0]);
	Location p2 = S1->isInsideStrip(S2->m_stripPoints[1]);
	Location p3 = S1->isInsideStrip(S2->m_stripPoints[2]);
	Location p4 = S1->isInsideStrip(S2->m_stripPoints[3]);

	const bool allLeft = isLeft(p1) && isLeft(p2) && isLeft(p3) && isLeft(p4);

	const bool allRight = isRight(p1) && isRight(p2) && isRight(p3) && isRight(p4);

	const bool allTop = isTop(p1) && isTop(p2) && isTop(p3) && isTop(p4);

	const bool allBottom = isBottom(p1) && isBottom(p2) && isBottom(p3) && isBottom(p4);

	return allLeft || allRight || allTop || allBottom;
}

IntersectionType StripTree::getIntersectionType(Strip *S1, Strip *S2)
{
	if(S1 == NULL || S2 == NULL)
	{
		return EmptySet;
	}

	// Check for inside
	const bool s1InsideS2 = isInside(S2, S1);
	const bool s2InsideS1 = isInside(S1, S2);
	
	assert(!s1InsideS2);
	assert(!s2InsideS1);
	   
	// Check for clear intersection
	const bool clearIntersection = checkForClearIntersection(S1, S2) ||
								   checkForClearIntersection(S2, S1);
	
	if(clearIntersection)
	{
		return ClearIntersection;
	}

	// Check for no intersection
	const bool noIntersection = checkForNoIntersection(S1, S2) ||
							   checkForNoIntersection(S2, S1);

	if(noIntersection)
	{
		return EmptySet;
	}

	return PossibleIntersection;
}

bool StripTree::intersects(int & countS1, int & countS2, Strip *S1, Strip *S2)
{
	if(S1 == NULL)
	{
		countS1--;
		return false;
	}
	if(S2 == NULL)
	{
		countS2--;
		return false;
	}


	IntersectionType intersection = getIntersectionType(S1, S2);

	switch(intersection)
	{
	case EmptySet:
		return false;
		break;
	case PossibleIntersection:
		if(S1->getArea() > S2->getArea())
		{
			return intersects(++countS1, countS2, S1->m_left, S2) ||
				   intersects(++countS1, countS2, S1->m_right, S2); 
		}
		else
		{
			return intersects(countS1, ++countS2, S1, S2->m_left) ||
				   intersects(countS1, ++countS2, S1, S2->m_right); 
		}
		break;
	case ClearIntersection:
		return true;
		break;
	default:
		assert(0);
	}
	return false;
}

bool StripTree::intersects(int & countS1, int & countS2, const StripTree & tree)
{
	countS1 = 1;
	countS2 = 1;
	return intersects(countS1, countS2, m_root, tree.m_root);
}

dnl::Polyline StripTree::getPolylineWithinTolerance(const double tolerance, Strip *S1)
{
	if(S1 == NULL)
	{
		return dnl::Polyline("");
	}
	if(S1->getWidth() <= tolerance)
	{
		dnl::Polyline line("");
		line.addPoint(S1->m_begin);
		line.addPoint(S1->m_end);
		return line;
	}

	if(S1->m_left == NULL && S1->m_right == NULL)
	{
		// Error, cannot get within tolerance required.
		assert(0);
	}

	dnl::Polyline line1 = getPolylineWithinTolerance(tolerance, S1->m_left);
	dnl::Polyline line2 = getPolylineWithinTolerance(tolerance, S1->m_right);

	dnl::Polyline line3("");
	for(unsigned int i = 0; i < line1.m_points.size(); i++)
	{
		line3.addPoint(line1.m_points[i]);
	}
	for(unsigned int i = 0; i < line2.m_points.size(); i++)
	{
		line3.addPoint(line2.m_points[i]);
	}
	
	// remove duplicate points
	dnl::Polyline line4("");
	Point prev;
	for(unsigned int i = 0; i < line3.m_points.size(); i++)
	{
		if(i == 0)
		{	
			line4.addPoint(line3.m_points[i]);
		}
		else
		{
			if(line3.m_points[i].m_x != prev.m_x && line3.m_points[i].m_y != prev.m_y)
			{
				line4.addPoint(line3.m_points[i]);
			}
		}
		prev = line3.m_points[i];
	}

	return line4;
}

dnl::Polyline StripTree::getPolylineWithinTolerance(const double tolerance)
{
	return getPolylineWithinTolerance(tolerance, m_root);
}