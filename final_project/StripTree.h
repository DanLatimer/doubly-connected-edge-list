
#pragma once

#include "common.h"
#include "Point.h"
#include "Strip.h"
#include "Polyline.h"

enum IntersectionType
{
	EmptySet = 0,
	ClearIntersection = 1,
	PossibleIntersection = 2
};

class StripTree
{
public:
	StripTree(const dnl::Polyline polyline, const double wStar);

	// Returns NULL on false, otherwise it returns the stip it determined in.
	Strip * pointWithinThresholdOfPolyline(
		const dnl::Point & p, 
		const double threashold);

	bool intersects(int & countS1, int & countS2, const StripTree & tree);
	dnl::Polyline getPolylineWithinTolerance(const double tolerance);

	int getSize();
	int getHeight();

private:
	Strip * createTree(const dnl::Polyline & polyline, const int pointL, const int pointR, const double wStar);

	Strip * pointWithinThresholdOfPolyline(
		Strip *node, 
		const dnl::Point & point, 
		const double threshold);

	IntersectionType getIntersectionType(Strip *S1, Strip *S2);
	bool intersects(int & countS1, int & countS2, Strip *S1, Strip *S2);
	dnl::Polyline getPolylineWithinTolerance(const double tolerance, Strip *S1);

	int getSize(Strip *node);
	int getHeight(Strip *node);

public:
	const dnl::Polyline m_polyline;
	Strip * m_root;

};