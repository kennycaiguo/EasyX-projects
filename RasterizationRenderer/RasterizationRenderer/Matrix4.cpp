#include "Matrix4.h"

Matrix4::Matrix4(double mat4[4][4])
{
	memcpy(value, mat4, sizeof(mat4));
}

Matrix4::Matrix4(const Matrix4& mat4)
{
	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			value[i][j] = mat4(i, j);
}

Matrix4::Matrix4(std::initializer_list<double> list)
{
	assert(list.size() >= 16);
	size_t i = 0;
	for (auto e : list)
	{
		size_t row = i / 4, column = i % 4;
		value[row][column] = e;
		i++;
	}
}

Matrix4& Matrix4::operator=(const Matrix4& mat4)
{
	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			value[i][j] = mat4(i, j);

	return *this;
}

Matrix4& Matrix4::operator=(std::initializer_list<double> list)
{
	assert(list.size() >= 16);
	size_t i = 0;
	for (auto e : list)
	{
		size_t row = i / 4, column = i % 4;
		value[row][column] = e;
		i++;
	}

	return *this;
}

inline double Matrix4::operator()(size_t i, size_t j) const
{
	assert(i >= 0 && i < 4 && j >= 0 && j < 4);
	return value[i][j];
}

inline double& Matrix4::operator()(size_t i, size_t j)
{
	assert(i >= 0 && i < 4 && j >= 0 && j < 4);
	return value[i][j];
}

Matrix4 Matrix4::operator*(const Matrix4& mat4) const
{
	Matrix4 result = {
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0
	};

	for (size_t row = 0; row < 4; row++)
		for (size_t column = 0; column < 4; column++)
			for (size_t i = 0; i < 4; i++)
				result(row, column) += value[row][i] * mat4(i, column);

	return result;
}

Vector4 Matrix4::operator*(const Vector4& vec4) const
{
	Vector4 result;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			result[i] = value[i][j] * vec4[j];
		}
	}
	return result;
}

Point4 Matrix4::operator*(const Point4& point) const
{
	Point4 result;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			result[i] = value[i][j] * point[j];
		}
	}
	return result;
}

Matrix4 Matrix4::Inverse(const Matrix4& mat4)
{
	// 利用伴随矩阵计算逆矩阵
	Matrix4 inverse;
	double a = mat4(0, 0);
	double b = mat4(1, 0);
	double c = mat4(2, 0);
	double d = mat4(3, 0);
	double e = mat4(0, 1);
	double f = mat4(1, 1);
	double g = mat4(2, 1);
	double h = mat4(3, 1);
	double i = mat4(0, 2);
	double j = mat4(1, 2);
	double k = mat4(2, 2);
	double l = mat4(3, 2);
	double m = mat4(0, 3);
	double n = mat4(1, 3);
	double o = mat4(2, 3);
	double p = mat4(3, 3);
	double q = a * f - b * e;
	double r = a * g - c * e;
	double s = a * h - d * e;
	double t = b * g - c * f;
	double u = b * h - d * f;
	double v = c * h - d * g;
	double w = i * n - j * m;
	double x = i * o - k * m;
	double y = i * p - l * m;
	double z = j * o - k * n;
	double A = j * p - l * n;
	double B = k * p - l * o;
	double reciprocalDet = 1 / (q * B - r * A + s * z + t * y - u * x + v * w);	// 如果行列式为0，结果为inf
	inverse(0, 0) = (f * B - g * A + h * z) * reciprocalDet;
	inverse(1, 0) = (-b * B + c * A - d * z) * reciprocalDet;
	inverse(2, 0) = (n * v - o * u + p * t) * reciprocalDet;
	inverse(3, 0) = (-j * v + k * u - l * t) * reciprocalDet;
	inverse(0, 1) = (-e * B + g * y - h * x) * reciprocalDet;
	inverse(1, 1) = (a * B - c * y + d * x) * reciprocalDet;
	inverse(2, 1) = (-m * v + o * s - p * r) * reciprocalDet;
	inverse(3, 1) = (i * v - k * s + l * r) * reciprocalDet;
	inverse(0, 2) = (e * A - f * y + h * w) * reciprocalDet;
	inverse(1, 2) = (-a * A + b * y - d * w) * reciprocalDet;
	inverse(2, 2) = (m * u - n * s + p * q) * reciprocalDet;
	inverse(3, 2) = (-i * u + j * s - l * q) * reciprocalDet;
	inverse(0, 3) = (-e * z + f * x - g * w) * reciprocalDet;
	inverse(1, 3) = (a * z - b * x + c * w) * reciprocalDet;
	inverse(2, 3) = (-m * t + n * r - o * q) * reciprocalDet;
	inverse(3, 3) = (i * t - j * r + k * q) * reciprocalDet;

	return inverse;
}

Matrix4 Matrix4::Scaling(Vector3& scale)
{
	double sx = scale.x;
	double sy = scale.y;
	double sz = scale.z;
	return Matrix4{
		sx, 0,  0,  0,
		0,  sy, 0,  0,
		0,  0,  sz, 0,
		0,  0,  0,  1
	};
}

Matrix4 Matrix4::Scaling(double sx, double sy, double sz)
{
	return Matrix4{
		sx, 0,  0,  0,
		0,  sy, 0,  0,
		0,  0,  sz, 0,
		0,  0,  0,  1
	};
}

Matrix4 Matrix4::Rotate(Vector3& r, double angle)
{
	double radian = angle * PI / 180;
	double x = r.x;
	double y = r.y;
	double z = r.z;
	double c = cos(radian);
	double s = sin(radian);

	return Matrix4{
		c + x * x * (1 - c),	 x * y * (1 - c) - z * s, x * z * (1 - c) + y * s, 0,
		y * x * (1 - c) + z * s, c + y * y * (1 - c),	  y * z * (1 - c) - x * s, 0,
		z * x * (1 - c) - y * s, z * y * (1 - c) + x * s, c + z * z * (1 - c),	   0,
		0,						 0,						  0,					   1
	};
}

Matrix4 Matrix4::Translate(Vector3& translation)
{
	double tx = translation.x;
	double ty = translation.y;
	double tz = translation.z;
	return Matrix4{
		1, 0, 0, tx,
		0, 1, 0, ty,
		0, 0, 1, tz,
		0, 0, 0, 1
	};
}

Matrix4 Matrix4::Translate(double tx, double ty, double tz)
{
	return Matrix4{
		1, 0, 0, tx,
		0, 1, 0, ty,
		0, 0, 1, tz,
		0, 0, 0, 1
	};
}
