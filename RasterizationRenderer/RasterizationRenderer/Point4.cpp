#include "Point4.h"

Point4::Point4(double x1, double y1, double z1, double w1)
{
	x = x1;
	y = y1;
	z = z1;
	w = w1;
}

Point4::Point4(std::initializer_list<double> list)
{
	assert(list.size() >= 4);
	auto it = list.begin();
	x = *it++;
	y = *it++;
	z = *it++;
	w = *it;
}

Point4& Point4::operator=(std::initializer_list<double> list)
{
	assert(list.size() >= 4);
	auto it = list.begin();
	x = *it++;
	y = *it++;
	z = *it++;
	w = *it;

	return *this;
}

double Point4::operator[](size_t i) const
{
	assert(i >= 0 && i < 4);
	switch (i)
	{
	case 0:
		return x; break;
	case 1:
		return y; break;
	case 2:
		return z; break;
	case 3:
		return w; break;
	default:
		return 0.0;
	}
}

double& Point4::operator[](size_t i)
{
	assert(i >= 0 && i < 4);
	switch (i)
	{
	case 0:
		return x; break;
	case 1:
		return y; break;
	case 2:
		return z; break;
	case 3:
		return w; break;
	}
}
