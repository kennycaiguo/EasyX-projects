#include "Point3.h"

Point3::Point3(double x1, double y1, double z1)
{
	x = x1;
	y = y1;
	z = z1;
}

Point3::Point3(std::initializer_list<double> list)
{
	assert(list.size() >= 3);
	auto it = list.begin();
	x = *it++;
	y = *it++;
	z = *it;
}

Point3& Point3::operator=(std::initializer_list<double> list)
{
	assert(list.size() >= 3);
	auto it = list.begin();
	x = *it++;
	y = *it++;
	z = *it;

	return *this;
}
