#include "Edge.h"

bool sortEdge(Edge const& edge1, Edge const& edge2)
{
	if (edge1.x != edge2.x)
		return edge1.x < edge2.x;
	else
		return edge1.dx < edge2.dx;	// 如果x坐标相等，那么根据x坐标的增量来比较，增量越大则在下一行x坐标也越大
}