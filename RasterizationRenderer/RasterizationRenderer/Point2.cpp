#include "Point2.h"

Point2::Point2(double x1, double y1)
{
	u = x = x1;
	v = y = y1;
}

Point2::Point2(std::initializer_list<double> list)
{
	assert(list.size() >= 2);
	auto it = list.begin();
	u = x = *it++;
	v = y = *it;
}

Point2& Point2::operator=(std::initializer_list<double> list)
{
	assert(list.size() >= 2);
	auto it = list.begin();
	u = x = *it++;
	v = y = *it;

	return *this;
}
