#pragma once

class Point3
{
public:
	Point3(double x1, double y1, double z1)
		: x(x1), y(y1), z(z1) {}
	Point3() { x = y = z = 0; }

	double x, y, z;
};