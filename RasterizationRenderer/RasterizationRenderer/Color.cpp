#include "Color.h"

Color::Color(double r1, double g1, double b1)
{
	r = r1;
	g = g1;
	b = b1;
}

Color::Color(std::initializer_list<double> list)
{
	assert(list.size() >= 3);
	auto it = list.begin();
	r = *it++;
	g = *it++;
	b = *it;
}

Color& Color::operator=(std::initializer_list<double> list)
{
	assert(list.size() >= 3);
	auto it = list.begin();
	r = *it++;
	g = *it++;
	b = *it;

	return *this;
}

Color Color::operator*(double a) const
{
	return Color(a * r, a * g, a * b);
}

Color Color::operator+(const Color& color) const
{
	return Color(r + color.r, g + color.g, b + color.b);
}

Color operator* (double a, Color& color)
{
	return color * a;
}