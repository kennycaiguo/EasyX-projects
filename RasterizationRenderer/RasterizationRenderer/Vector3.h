#pragma once

#include <cmath>
#include <initializer_list>
#include <cassert>

class Vector3
{
public:
	Vector3(double x1 = 0.0, double y1 = 0.0, double z1 = 0.0);
	Vector3(std::initializer_list<double> list);
	~Vector3() {}

	Vector3& operator=(std::initializer_list<double> list);
	Vector3 operator-(Vector3& a);
	double operator[](size_t i) const;
	double& operator[](size_t i);

	// 向量的模
	inline double Mod() const;

	static Vector3 Cross(Vector3& a, Vector3& b);
	static double Dot(Vector3& a, Vector3& b);
	void Normalize();

	double x, y, z;
};