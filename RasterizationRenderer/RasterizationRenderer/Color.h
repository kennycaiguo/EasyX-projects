#pragma once

#include <initializer_list>
#include <cassert>

class Color
{
public:
	Color(double r1 = 0.0, double g1 = 0.0, double b1 = 0.0);
	Color(std::initializer_list<double> list);
	~Color() {}

	Color& operator=(std::initializer_list<double> list);

	Color operator*(double a) const;
	Color operator+(const Color& color) const;

	double r, g, b;
};

Color operator* (double a, Color& color);