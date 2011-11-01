
#pragma once

#include "common.h"
#include "Point.h"

class utils
{
public:

	// line functions
	static double distanceOfPointFromLine(const dnl::Point & begin, const dnl::Point & end, const dnl::Point & point);
	static bool pointToRightOfLine(const dnl::Point & begin, const dnl::Point & end, const dnl::Point & point);

private:
	// Helper functions
	static double getSignedDistanceOfPointFromLine(const dnl::Point & begin, const dnl::Point & end, const dnl::Point & point);
};