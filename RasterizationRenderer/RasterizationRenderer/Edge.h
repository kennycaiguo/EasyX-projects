#pragma once

class Edge
{
public:
	Edge(double x1 = 0, double dx1 = 0, double yMax1 = 0)
		: x(x1), dx(dx1), yMax(yMax1) {}
	~Edge() {}

	double x, dx, yMax;
};

bool sortEdge(Edge const& edge1, Edge const& edge2);