#include "DisjointSet.h"

DisjointSet::DisjointSet(int _rows, int _columns)
    : rows(_rows), columns(_columns),
    parent(std::vector<Vector2i>(rows * columns)),
    rank(std::vector<int>(rows * columns, 0))
{
	for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; j++)
		    parent[i * columns + j] = Vector2i(i, j);
}

Vector2i DisjointSet::Find(Vector2i v)
{
    int index = v.x() * columns + v.y();
	return v == parent[index] ? v : (parent[index] = Find(parent[index]));
}

void DisjointSet::Union(Vector2i v1, Vector2i v2)
{
    Vector2i p1 = Find(v1);
    int index1 = p1.x() * columns + p1.y();
    Vector2i p2 = Find(v2);
    int index2 = p2.x() * columns + p2.y();
    if (rank[index1] > rank[index2])
        parent[index2] = p1;
    else
    {
        parent[index1] = p2;
        if (rank[index1] == rank[index2])
            ++rank[index2];
    }
}

bool DisjointSet::IsSame(Vector2i v1, Vector2i v2)
{
    return Find(v1) == Find(v2);
}
