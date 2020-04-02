#pragma once

#include <initializer_list>
#include <cassert>

class Point3
{
public:
	Point3(double x1 = 0.0, double y1 = 0.0, double z1 = 0.0);
	Point3(std::initializer_list<double> list);

	Point3& operator=(std::initializer_list<double> list);

	double x, y, z;
};