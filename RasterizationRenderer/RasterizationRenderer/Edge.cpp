#include "Edge.h"

bool sortEdge(Edge const& edge1, Edge const& edge2)
{
	if (edge1.x != edge2.x)
		return edge1.x < edge2.x;
	else
		return edge1.dx < edge2.dx;	// ���x������ȣ���ô����x������������Ƚϣ�����Խ��������һ��x����ҲԽ��
}