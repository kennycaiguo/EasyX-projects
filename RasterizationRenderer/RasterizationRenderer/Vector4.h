#pragma once

#include <cmath>
#include <initializer_list>
#include <cassert>
#include "Vector3.h"

class Vector4
{
public:
	Vector4(double x1 = 0.0, double y1 = 0.0, double z1 = 0.0, double w1 = 0.0);
	Vector4(Vector3& vec, double w);
	Vector4(std::initializer_list<double> list);
	~Vector4() {}

	Vector4& operator=(std::initializer_list<double> list);
	double operator[](size_t i) const;
	double& operator[](size_t i);

	double x, y, z, w;
};