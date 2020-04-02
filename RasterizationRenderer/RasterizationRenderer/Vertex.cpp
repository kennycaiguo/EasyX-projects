#include "Vertex.h"

Vertex::Vertex(double x, double y, double z, double u, double v, double r, double g, double b)
{
	position.x = x, position.y = y, position.z = z;
	texCoord.u = u, texCoord.v = v;
	color.r = r, color.g = g, color.b = b;
}

Vertex::Vertex(Point3& pos, Point2& tex, Color& c)
{
	position = pos;
	texCoord = tex;
	color = c;
}

Vertex::Vertex(std::initializer_list<double> list)
{
	assert(list.size() >= 8);
	auto it = list.begin();
	position.x = *it++;
	position.y = *it++;
	position.z = *it++;
	texCoord.u = *it++;
	texCoord.v = *it++;
	color.r = *it++;
	color.g = *it++;
	color.b = *it;
}

Vertex& Vertex::operator=(std::initializer_list<double> list)
{
	assert(list.size() >= 8);
	auto it = list.begin();
	position.x = *it++;
	position.y = *it++;
	position.z = *it++;
	texCoord.u = *it++;
	texCoord.v = *it++;
	color.r = *it++;
	color.g = *it++;
	color.b = *it;

	return *this;
}

