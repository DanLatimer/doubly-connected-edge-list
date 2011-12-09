
#pragma once

#include "common.h"
#include "Point.h"

class utils
{
public:

	// line functions
	static double distanceOfPointFromLine(const dnl::Point & begin, const dnl::Point & end, const dnl::Point & point);
	static bool pointToRightOfLine(const dnl::Point & begin, const dnl::Point & end, const dnl::Point & point);

	static double parseDouble(const std::string &data);
	static void expandBoundingBox(dnl::Point &llBox, dnl::Point &urBox, const dnl::Point &llCurrent, const dnl::Point &urCurrent);
	static void expandBoundingBox(dnl::Point &llBox, dnl::Point &urBox, const dnl::Point &current);

	static std::wstring StringToWString(const std::string& s);
	static std::string WStringToString(const std::wstring& s);

	static void setTextOnStatusBar(const std::wstring &str);
private:
	// Helper functions
	static double getSignedDistanceOfPointFromLine(const dnl::Point & begin, const dnl::Point & end, const dnl::Point & point);
};