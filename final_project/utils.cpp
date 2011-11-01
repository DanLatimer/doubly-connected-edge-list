
#include "stdafx.h"
#include "utils.h"
#include "Point.h"
#include "math.h"

using namespace dnl;

double utils::getSignedDistanceOfPointFromLine(const Point & begin, const Point & end, const Point & point)
{
	// Get Vector from begin to end.
	Point vectorBE(end.m_x - begin.m_x, end.m_y - begin.m_y);
	
	// Get perpendicular vector.
	Point perpVectorBE(vectorBE.m_y * (-1), vectorBE.m_x);
	Point unitPerpVectorBE(perpVectorBE.getUnitVector());

	// Get vector from point to begin.
	//Point vectorPB(begin.m_x - point.m_x, begin.m_y - point.m_y);
	Point vectorPB(point.m_x - begin.m_x, point.m_y - begin.m_y);

	// Calculate the dot product
	const double distance = unitPerpVectorBE.getDotProduct(vectorPB);

	const double distance2 = vectorPB.getDotProduct(perpVectorBE) / perpVectorBE.getVectorMagnitude();
	assert(pow(distance - distance2, 2) < 0.000001); // same within a tolerance

	return distance;
}

double utils::distanceOfPointFromLine(const Point & begin, const Point & end, const Point & point)
{
	double signedDistance = getSignedDistanceOfPointFromLine(begin, end, point);
	return sqrt(signedDistance*signedDistance);
}

bool utils::pointToRightOfLine(const Point & begin, const Point & end, const Point & point)
{
	double signedDistance = getSignedDistanceOfPointFromLine(begin, end, point);
	return signedDistance < 0;
}