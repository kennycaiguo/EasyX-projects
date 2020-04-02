#pragma once

#include <initializer_list>
#include <cassert>

class Point4
{
public:
	Point4(double x1 = 0.0, double y1 = 0.0, double z1 = 0.0, double w1 = 0.0);
	Point4(std::initializer_list<double> list);

	Point4& operator=(std::initializer_list<double> list);
	double operator[](size_t i) const;
	double& operator[](size_t i);

	double x, y, z, w;
};