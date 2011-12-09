
#include "stdafx.h"
#include "utils.h"
#include "Point.h"
#include "math.h"
#include <sstream>
#include "CommCtrl.h"

using namespace dnl;
using namespace std;

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

double utils::parseDouble(const std::string &data)
{
	std::stringstream stream(data);
	double retVal;
	stream >> retVal;
	return retVal;
}

void utils::expandBoundingBox(dnl::Point &llBox, dnl::Point &urBox, const dnl::Point &llCurrent, const dnl::Point &urCurrent)
{
	if(llCurrent.m_x < llBox.m_x)
	{
		llBox.m_x = llCurrent.m_x;
	}

	if(llCurrent.m_y < llBox.m_y)
	{
		llBox.m_y = llCurrent.m_y;
	}

	if(urCurrent.m_x > urBox.m_x)
	{
		urBox.m_x = urCurrent.m_x;
	}

	if(urCurrent.m_y > urBox.m_y)
	{
		urBox.m_y = urCurrent.m_y;
	}
}

void utils::expandBoundingBox(dnl::Point &llBox, dnl::Point &urBox, const dnl::Point &current)
{
	if(current.m_x < llBox.m_x)
	{
		llBox.m_x = current.m_x;
	}

	if(current.m_y < llBox.m_y)
	{
		llBox.m_y = current.m_y;
	}

	if(current.m_x > urBox.m_x)
	{
		urBox.m_x = current.m_x;
	}

	if(current.m_y > urBox.m_y)
	{
		urBox.m_y = current.m_y;
	}
}

std::wstring utils::StringToWString(const std::string& s)
{
	std::wstring temp(s.length(),L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp; 
}

std::string utils::WStringToString(const std::wstring& s)
{
	std::string temp(s.length(), ' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp; 
}

void utils::setTextOnStatusBar(const std::wstring &str)
{
	if(statusBarSet == false)
	{
		return;
	}

	wchar_t szBuf[1000] = {0};
	wsprintf(szBuf, str.c_str());
    SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)(LPWSTR)szBuf);
}