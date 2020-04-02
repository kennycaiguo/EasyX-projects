#pragma once

#include <initializer_list>
#include <cassert>

class Point2
{
public:
	Point2(double x1 = 0, double y1 = 0);
	Point2(std::initializer_list<double> list);
	~Point2() {}

	Point2& operator=(std::initializer_list<double> list);

	double x, y;
	double u, v;
};