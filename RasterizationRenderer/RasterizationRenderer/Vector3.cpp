#include "Vector3.h"

Vector3::Vector3(double x1, double y1, double z1)
{
	x = x1;
	y = y1;
	z = z1;
}

Vector3::Vector3(std::initializer_list<double> list)
{
	assert(list.size() >= 3);
	auto it = list.begin();
	x = *it++;
	y = *it++;
	z = *it;
}

Vector3& Vector3::operator=(std::initializer_list<double> list)
{
	assert(list.size() >= 3);
	auto it = list.begin();
	x = *it++;
	y = *it++;
	z = *it;

	return *this;
}

Vector3 Vector3::operator+(Vector3 a)
{
	return Vector3(x + a.x, y + a.y, z + a.z);
}

Vector3& Vector3::operator+=(Vector3 a)
{
	x += a.x;
	y += a.y;
	z += a.z;
	return *this;
}

Vector3& Vector3::operator-=(Vector3 a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
}

Vector3 Vector3::operator-(Vector3 a)
{
	return Vector3(x - a.x, y - a.y, z - a.z);
}

Vector3 Vector3::operator*(double a)
{
	return Vector3(x * a, y * a, z * a);
}

double Vector3::operator[](size_t i) const
{
	assert(i >= 0 && i < 3);
	switch (i)
	{
	case 0:
		return x; break;
	case 1:
		return y; break;
	case 2:
		return z; break;
	default:
		return 0.0;
	}
}

double& Vector3::operator[](size_t i)
{
	assert(i >= 0 && i < 3);
	switch (i)
	{
	case 0:
		return x; break;
	case 1:
		return y; break;
	case 2:
		return z; break;
	}
}

Vector3 Vector3::Cross(Vector3& a, Vector3& b)
{
	return Vector3((a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x));
}

double Vector3::Dot(Vector3& a, Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void Vector3::Normalize()
{
	double mod = Mod();
	x /= mod;
	y /= mod;
	z /= mod;
}

double Vector3::Mod() const
{
	return sqrt(x * x + y * y + z * z);
}


