#pragma once

class Point2
{
public:
	Point2();
	Point2(double x1, double y1);
	~Point2() {}

	double x, y;
};

Point2::Point2()
{
	x = y = 0;
}

Point2::Point2(double x1, double y1)
	: x(x1), y(y1)
{
}