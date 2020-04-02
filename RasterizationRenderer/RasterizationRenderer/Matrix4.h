#pragma once

#include <cassert>
#include <cstring>
#include <initializer_list>
#include "Vector3.h"
#include "Vector4.h"
#include "Point4.h"

const double PI = 3.1415926;

class Matrix4
{
public:
	Matrix4() {}
	Matrix4(double mat4[4][4]);
	Matrix4(std::initializer_list<double> list);
	Matrix4(const Matrix4& mat4);
	~Matrix4() {}

	Matrix4& operator=(const Matrix4& mat4);
	Matrix4& operator=(std::initializer_list<double> list);

	inline double operator()(size_t i, size_t j) const;
	inline double& operator()(size_t i, size_t j);

	Matrix4 operator*(const Matrix4& mat4) const;		// 矩阵乘法
	Vector4 operator*(const Vector4& vec4) const;		// 矩阵与向量乘法
	Point4 operator*(const Point4& point) const;		// 矩阵与点乘法

	static Matrix4 Inverse(const Matrix4& mat4);
	static Matrix4 Scaling(Vector3& scale);
	static Matrix4 Scaling(double sx, double sy, double sz);
	static Matrix4 Rotate(Vector3& axis, double angle);
	static Matrix4 Translate(Vector3& translation);
	static Matrix4 Translate(double tx, double ty, double tz);

private:
	double value[4][4] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
};