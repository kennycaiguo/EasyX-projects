#pragma once

#include <cmath>

class Vector3
{
public:
	Vector3(double x1 = 0, double y1 = 0, double z1 = 0)
		: x(x1), y(y1), z(z1) {}
	~Vector3() {}

	double x, y, z;

	// 向量的长度
	double length() const
	{
		return sqrt(x * x + y * y + z * z);
	}
};
// 向量叉乘
Vector3 cross(Vector3& a, Vector3& b);


class Color
{
public:
	Color(float r1 = 0.0f, float g1 = 0.0f, float b1 = 0.0f)
		: r(r1), g(g1), b(b1) {}
	~Color() {}

	float r, g, b;

	Color operator* (float a)
	{
		return Color(a * r, a * g, a * b);
	}

	Color operator+ (Color color)
	{
		return Color(r + color.r, g + color.g, b + color.b);
	}
};

Color operator* (float a, Color& color);

