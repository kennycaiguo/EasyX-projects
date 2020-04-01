#pragma once

class Point4
{
public:
	Point4(double x1, double y1, double z1, double w1)
		: x(x1), y(y1), z(z1), w(w1) {}
	Point4() { x = y = z = w = 0; }

	double x, y, z, w;
};