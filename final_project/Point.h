
#pragma once

namespace dnl
{

class Point
{
public:
	Point() : m_x(0), m_y(0) { }
	Point(double x, double y) : m_x(x), m_y(y) { }

	void output();

	// point functions
	double getDistanceBetween(const Point & other) const;

	// vector functions
	double getVectorMagnitude();
	Point getUnitVector();
	double getDotProduct(const Point & other);

	double m_x;
	double m_y;

	Point operator*(const double & other);
	Point operator/(const double & other);
	Point operator+(const Point & other);
	Point operator-(const Point & other);

};

}