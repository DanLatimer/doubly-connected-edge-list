
#pragma once

#include "common.h"
#include "Point.h"
#include "Polyline.h"
#include "PrintManager.h"

/* Location enumeration (graphical representation):

    2 | 5 | 8
    ----------
    1 | 4 | 7
    ----------
    0 | 3 | 6

*/
enum Location
{
	LEFT_BOTTOM = 0,
	LEFT_MIDDLE = 1,
	LEFT_TOP = 2,
	CENTRE_BOTTOM = 3,
	CENTRE_MIDDLE = 4, // Inside strip
	CENTRE_TOP = 5,
	RIGHT_BOTTOM = 6,
	RIGHT_MIDDLE = 7,
	RIGHT_TOP = 8
};

class Strip
{
public:

	Strip(const dnl::Point & begin, 
		  const dnl::Point & end, 
		  double widthRight, 
		  double widthLeft); 

	double getWidth() const;
	double getArea() const;

	void print(PrintManager &printMan);

	double distanceFromStrip(const dnl::Point & point);
	double upperBound(const dnl::Point & point);
	double lowerBound(const dnl::Point & point);

	Location isInsideStrip(const dnl::Point & point) const;

	dnl::Point m_begin;
	dnl::Point m_end;
	double m_widthRight;
	double m_widthLeft;
	Strip * m_left;
	Strip * m_right;

	std::vector<dnl::Point> m_stripPoints;
	// [0] - left of B
	// [1] - right of B
	// [2] - left of E
	// [3] - right of E

};