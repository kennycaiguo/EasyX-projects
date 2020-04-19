#pragma once

#include <vector>
#include <Eigen/Dense>
using namespace Eigen;

class DisjointSet
{
public:
	DisjointSet(int _rows, int _columns);

	Vector2i Find(Vector2i v);				// 查找根节点
	void Union(Vector2i v1, Vector2i v2);	// 合并两个节点
	bool IsSame(Vector2i v1, Vector2i v2);	// 两个节点是否在一个集合中

private:
	int rows, columns;
	std::vector<Vector2i> parent;	// 节点的父母节点
	std::vector<int> rank;			// 每个集合的秩
};