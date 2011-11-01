
#include "stdafx.h"
#include "common.h"
#include "Strip.h"
#include "Point.h"
#include "utils.h"
#include "math.h"

#include "PrintManager.h"

using namespace std;

void getStripPoints(
	vector<dnl::Point> & points, 
	const dnl::Point & begin, 
	dnl::Point & end, // TODO: write const Point - operator so this can be const again.
	const double widthRight,
	const double widthLeft)
{
	// Get vector representing begin to end.
	dnl::Point vectorBE(end - begin);

	// Get its unit vector
	dnl::Point unitVectorBE(vectorBE.getUnitVector());

	// Get vector from B to P1.
	dnl::Point vectorBP1(unitVectorBE.m_y * (-widthLeft), unitVectorBE.m_x * widthLeft);

	// Get P1 and add to result set.
	dnl::Point p1(vectorBP1 + begin);
	points.push_back(p1);

	// Get vector from B to P2.
	dnl::Point vectorBP2(unitVectorBE.m_y * widthRight, unitVectorBE.m_x * (-widthRight));

	// Get P2 and add to result set.
	dnl::Point p2(vectorBP2 + begin);
	points.push_back(p2);

	// Get P3 and add to result set.
	dnl::Point p3(p1 + vectorBE);
	points.push_back(p3);

	// Get P4 and add to result set.
	dnl::Point p4(p2 + vectorBE);
	points.push_back(p4);
}

Strip::Strip(const dnl::Point & begin, 
	  const dnl::Point & end, 
	  double widthRight, 
	  double widthLeft)
		: m_begin(begin), 
		  m_end(end), 
		  m_widthRight(widthRight), 
		  m_widthLeft(widthLeft), 
		  m_left(NULL), 
		  m_right(NULL) 
{
	getStripPoints(m_stripPoints, m_begin, m_end, m_widthRight, m_widthLeft);
}

double Strip::getWidth() const 
{
	return m_widthLeft + m_widthRight;
}

double Strip::distanceFromStrip(const dnl::Point & point)
{
	Location location = isInsideStrip(point);
	switch(location)
	{
	case LEFT_BOTTOM:
		return m_stripPoints[1].getDistanceBetween(point);
		break;
	case LEFT_MIDDLE:
		return utils::distanceOfPointFromLine(m_stripPoints[0], m_stripPoints[1], point);
		break;
	case LEFT_TOP:
		return m_stripPoints[0].getDistanceBetween(point);
		break;
	case CENTRE_BOTTOM:
		return utils::distanceOfPointFromLine(m_stripPoints[1], m_stripPoints[3], point);
		break;
	case CENTRE_MIDDLE:
		assert(0);
		break;
	case CENTRE_TOP:
		return utils::distanceOfPointFromLine(m_stripPoints[0], m_stripPoints[2], point);
		break;
	case RIGHT_BOTTOM:
		return m_stripPoints[3].getDistanceBetween(point);
		break;
	case RIGHT_MIDDLE:
		return utils::distanceOfPointFromLine(m_stripPoints[2], m_stripPoints[3], point);
		break;
	case RIGHT_TOP:
		return m_stripPoints[2].getDistanceBetween(point);
		break;
	default:
		assert(0);
	}

	return -1;
}

double Strip::upperBound(const dnl::Point & point)
{
	Location location = isInsideStrip(point);
	if(location == CENTRE_MIDDLE)
	{
		return getWidth();
	}
	else
	{
		return distanceFromStrip(point) + getWidth();
	}
}

double Strip::lowerBound(const dnl::Point & point)
{
	Location location = isInsideStrip(point);
	if(location == CENTRE_MIDDLE)
	{
		return 0;
	}
	else
	{
		return distanceFromStrip(point);
	}
}

// Gets a base 10 representation of four bits.
int getCaseNumber(bool A, bool B, bool C, bool D)
{
	int result = 0;

	result += A ? 8 : 0;
	result += B ? 4 : 0;
	result += C ? 2 : 0;
	result += D ? 1 : 0;

	return result;
}

/* Lines:
A - P1 to P2
B - P2 to P3
C - P3 to P4
D - P4 to P5

CASE | A | B | C | D | Result
----------------------
0    | 0 | 0 | 0 | 0 | ERROR
1    | 0 | 0 | 0 | 1 | ERROR
2    | 0 | 0 | 1 | 0 | ERROR
3    | 0 | 0 | 1 | 1 | RIGHT_TOP (8) 
4    | 0 | 1 | 0 | 0 | ERROR
5    | 0 | 1 | 0 | 1 | ERROR
6    | 0 | 1 | 1 | 0 | LEFT_TOP (2)
7    | 0 | 1 | 1 | 1 | CENTRE_TOP (5)
8    | 1 | 0 | 0 | 0 | ERROR
9    | 1 | 0 | 0 | 1 | RIGHT_BOTTOM (6)
10   | 1 | 0 | 1 | 0 | ERROR
11   | 1 | 0 | 1 | 1 | RIGHT_MIDDLE (7)
12   | 1 | 1 | 0 | 0 | LEFT_BOTTOM (0)
13   | 1 | 1 | 0 | 1 | CENTRE_BOTTOM (3)
14   | 1 | 1 | 1 | 0 | LEFT_MIDDLE (1)
15   | 1 | 1 | 1 | 1 | CENTRE_MIDDLE (4)

*/
Location Strip::isInsideStrip(const dnl::Point & point) const
{
	bool A, B, C, D;
	if(getWidth() == 0)
	{
		// Get perp vector
		dnl::Point pointBE(m_stripPoints[2].m_x - m_stripPoints[0].m_x, m_stripPoints[2].m_y - m_stripPoints[0].m_y);
		dnl::Point perpPointBE(pointBE.m_y, pointBE.m_x * -1);
		A = !utils::pointToRightOfLine(m_stripPoints[2], m_stripPoints[0], point);
		B = !utils::pointToRightOfLine(perpPointBE + m_stripPoints[0], m_stripPoints[0], point);
		C = !A;
		D = !utils::pointToRightOfLine(perpPointBE + m_stripPoints[2], m_stripPoints[2], point);
	}
	else
	{
		A = !utils::pointToRightOfLine(m_stripPoints[2], m_stripPoints[0], point);
		B = !utils::pointToRightOfLine(m_stripPoints[3], m_stripPoints[2], point);
		C = !utils::pointToRightOfLine(m_stripPoints[1], m_stripPoints[3], point);
		D = !utils::pointToRightOfLine(m_stripPoints[0], m_stripPoints[1], point);
	}

	const int caseNumber = getCaseNumber(A, B, C, D);
	switch(caseNumber)
	{
		case 12:
			return LEFT_BOTTOM;
		case 14:
			return LEFT_MIDDLE;
		case 6:
			return LEFT_TOP;
		case 13:
			return CENTRE_BOTTOM;
		case 15:
			return CENTRE_MIDDLE;
		case 7:
			return CENTRE_TOP;
		case 9:
			return RIGHT_BOTTOM;
		case 3:
			return RIGHT_TOP;
		case 11:
			return RIGHT_MIDDLE;
		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 8:
		case 10:
		default:
			ReportError("Impossible case found when determining point's relation to strip.");
			return LEFT_BOTTOM;
	}
}

void Strip::print(PrintManager & printMan)
{
	// Print box
	printMan.PrintLine(m_stripPoints[0], m_stripPoints[2]);
	printMan.PrintLine(m_stripPoints[2], m_stripPoints[3]);
	printMan.PrintLine(m_stripPoints[3], m_stripPoints[1]);
	printMan.PrintLine(m_stripPoints[1], m_stripPoints[0]);

	// Print main line
	printMan.PrintLine(m_begin, m_end);

	// Print arrow
	printMan.PrintArrow(m_begin, m_end);
}

double Strip::getArea() const
{
	const double width = getWidth();
	const double length = m_end.getDistanceBetween(m_begin);
	return width * length;
}