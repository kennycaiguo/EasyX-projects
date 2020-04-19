#pragma once

#include <vector>
#include <Eigen/Dense>
using namespace Eigen;

class DisjointSet
{
public:
	DisjointSet(int _rows, int _columns);

	Vector2i Find(Vector2i v);				// ���Ҹ��ڵ�
	void Union(Vector2i v1, Vector2i v2);	// �ϲ������ڵ�
	bool IsSame(Vector2i v1, Vector2i v2);	// �����ڵ��Ƿ���һ��������

private:
	int rows, columns;
	std::vector<Vector2i> parent;	// �ڵ�ĸ�ĸ�ڵ�
	std::vector<int> rank;			// ÿ�����ϵ���
};