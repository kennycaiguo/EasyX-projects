#pragma once

#include "Point2.h"
#include "Point3.h"
#include "Color.h"
#include <initializer_list>
#include <cassert>

class Vertex
{
public:
	Vertex(double x = 0.0, double y = 0.0, double z = 0.0,
		double u = 0.0, double v = 0.0,
		double r = 0.0, double g = 0.0, double b = 0.0);
	Vertex(Point3& pos, Point2& tex, Color& c);
	Vertex(std::initializer_list<double> list);

	Vertex& operator=(std::initializer_list<double> list);

	Point3 position;
	Point2 texCoord;
	Color color;
};