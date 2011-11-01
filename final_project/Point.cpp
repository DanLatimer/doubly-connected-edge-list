
#include "stdafx.h"
#include "Point.h"
#include "common.h"
#include "math.h"

using namespace std;

void dnl::Point::output()
{
	cout << " (" << m_x << ", " << m_y << ")" << endl;
}

double dnl::Point::getDistanceBetween(const dnl::Point & other) const 
{
	double xdiff = other.m_x - m_x;
	double ydiff = other.m_y - m_y;
	return sqrt(xdiff*xdiff + ydiff*ydiff);
}

double dnl::Point::getVectorMagnitude()
{
	return sqrt((m_x * m_x) + (m_y * m_y));
}

dnl::Point dnl::Point::getUnitVector()
{
	double magnitude = getVectorMagnitude();
	return dnl::Point(this->m_x / magnitude, this->m_y / magnitude);
}

double dnl::Point::getDotProduct(const dnl::Point & other)
{
	return m_x*other.m_x + m_y*other.m_y;
}

// Operator overloading

dnl::Point dnl::Point::operator*(const double & other)
{
	return Point(this->m_x * other, this->m_y * other);
}

dnl::Point dnl::Point::operator/(const double & other)
{
	return dnl::Point(this->m_x / other, this->m_y / other);
}

dnl::Point dnl::Point::operator+(const dnl::Point& other)
{
	return dnl::Point(this->m_x + other.m_x, this->m_y + other.m_y);
}

dnl::Point dnl::Point::operator-(const dnl::Point & other)
{
	return dnl::Point(this->m_x - other.m_x, this->m_y - other.m_y);
}