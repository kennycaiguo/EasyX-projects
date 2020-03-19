#include "Canvas.h"
#include "Point2.h"
#include <cstdio>
#include <vector>
#include <list>

// ����εı�
struct Edge
{
	double x;			// �ߵ����x����
	double dx;			// ɨ����ÿ�����ƶ�һ��x������
	double ymax;		// �ߵ��յ�y����
	Edge(double x1, double dx1, double ymax1)
		: x(x1), dx(dx1), ymax(ymax1) {}
};
// ��������
bool sortEdge(Edge const& edge1, Edge const& edge2)
{
	if (edge1.x != edge2.x)
		return edge1.x < edge2.x;
	else					
		return edge1.dx < edge2.dx;	// ���x������ȣ���ô����x������������Ƚϣ�����Խ��������һ��x����ҲԽ��
}

void fillPolygon(Canvas& canvas, std::vector<Point2>& points)
{
	if (points.size() < 3)
		return;

	// ����NET(new edge table)��
	std::list<Edge>* NET = new std::list<Edge>[canvas.mScrHeight];

	int yMin = (int)points[0].y;			// �����yֵ����Сֵ
	int yMax = (int)points[0].y;			// �����yֵ�����ֵ
	// ��ʼ��NET��
	int count = points.size();
	for (int i = 0; i < count; i++)
	{
		if (points[i].y > points[(i + 1) % count].y)
		{
			double x = points[(i + 1) % count].x;
			double dx = (points[(i + 1) % count].x - points[i].x)
				/ (points[(i + 1) % count].y - points[i].y);
			double ymax = points[i].y;
			NET[(int)points[(i + 1) % count].y].push_back(Edge(x, dx, ymax));
		}
		else if (points[i].y < points[(i + 1) % count].y)
		{
			double x = points[i].x;
			double dx = (points[(i + 1) % count].x - points[i].x)
				/ (points[(i + 1) % count].y - points[i].y);
			double ymax = points[(i + 1) % count].y;
			NET[(int)points[i].y].push_back(Edge(x, dx, ymax));
		}
		yMin = (int)min(points[i].y, yMin);
		yMax = (int)max(points[i].y, yMax);
	}

	// ����AEL(active edge list)����
	std::list<Edge> AEL;
	for (int y = yMin; y <= yMax; y++)
	{
		if (!NET[y].empty())								// ���ɨ���д��ڱߣ������AEL��
			AEL.splice(AEL.end(), NET[y]);
		AEL.sort(sortEdge);

		std::list<Edge>::iterator edgeStar, edgeEnd;		// ɨ������Ҫ���Ƶ��߶εĶ˵�ĵ�����
		int countEdge = 0;									// �Ѵ洢�Ķ˵�

		for (auto it = AEL.begin(); it != AEL.end(); )
		{
			// ɨ�����Ѿ�����ĳ���ߵ�y�����ֵ�����Ƴ��ñ�
			if ((int)it->ymax <= y)
			{
				it = AEL.erase(it);							// �����¼�Ƴ�Ԫ�ص���һ��Ԫ�صĵ�������������it++����Ϊit�Ѿ���Ч�ˡ�
			}
			else
			{
				if (countEdge == 0)
				{
					edgeStar = it;
					countEdge++;
				}
				else if (countEdge == 1)
				{
					countEdge = 0;
					edgeEnd = it;
					// �����߶�
					for (int x = (int)edgeStar->x; x <= (int)edgeEnd->x; x++)
						canvas.setPixel(x, y, WHITE);
					// ��Խ����xֵ����dx
					edgeStar->x += edgeStar->dx;
					edgeEnd->x += edgeEnd->dx;
				}
				it++;
			}
		}
	}
	delete[] NET;
}

/*
ʹ��ֱ�߲ü�һ��ֱ��
����һ��͹�������˵�������еĶ��㶼������һ���ߵ�ͬһ�࣬���Ե�ʹ�øö���ε�ĳһ������Ϊ�ü��߽磬��ѡ�����ⲻ������������Ķ���r����Ϊ�ü���������
�����ü�����p��r����ֱ�ߵ�ͬһ��ʱ˵��p�ᱻ�ü���
����������ü���Line,���޸Ĵ������line�����꣬�޸�A��ʱ����1,�޸�B��ʱ����2��ƽ������ʱ����0��ƽ���ܾ�����-1
�ü�ֱ�ߵĶ˵�ֱ�ΪBoundary0��Boundary1�����ü�ֱ��Ϊline�����������˵�
*/
int clipLL(std::vector<Point2>& boundary, Point2& reference, std::vector<Point2>& line)
{
	// �ü�ֱ�ߵ�һ��ʽϵ��
	double A1 = boundary[1].y - boundary[0].y;
	double B1 = boundary[0].x - boundary[1].x;
	double C1 = boundary[1].x * boundary[0].y - boundary[0].x * boundary[1].y;

	// ���ü�ֱ�߶˵��Ƿ񱻲ü�
	bool clipA = false , clipB = false;

	if ((A1 * reference.x + B1 * reference.y + C1) * (A1 * line[0].x + B1 * line[0].y + C1) < 0)
		clipA = true;
	if ((A1 * reference.x + B1 * reference.y + C1) * (A1 * line[1].x + B1 * line[1].y + C1) < 0)
		clipB = true;

	if (!clipA && !clipB)		// ƽ������
		return 0;
	else if (clipA && clipB)	// ƽ���ܾ�
		return -1;

	// ���ü�ֱ�ߵ�һ��ʽϵ��
	double A2 = line[1].y - line[0].y;
	double B2 = line[0].x - line[1].x;
	double C2 = line[1].x * line[0].y - line[0].x * line[1].y;
	// �ü�ֱ���뱻�ü�ֱ�ߵĽ���
	double x = (C2 * B1 - C1 * B2) / (A1 * B2 - A2 * B1);
	double y = (C1 * A2 - C2 * A1) / (A1 * B2 - A2 * B1);

	if (clipA)	
	{
		line[0].x = x;	// ���
		line[0].y = y;
		return 1;
	}
	else
	{
		line[1].x = x;	// ����
		line[1].y = y;
		return 2;
	}
}

/*
ʹ��һ��͹�������Ϊ�ü�����
*/
std::vector<Point2> clip(std::vector<Point2>& clipWindow, std::vector<Point2>& polygon)
{
	std::vector<Point2> res = polygon;
	int countOfpolygon = polygon.size();
	int countOfwindow = clipWindow.size();

	for (int j = 0; j < countOfwindow; j++)
	{
		std::vector<Point2> points;
		int size = res.size();

		for (int i = 0; i < size; i++)
		{
			std::vector<Point2> line = { res[i], res[(i + 1) % countOfpolygon] };
			std::vector<Point2> boundary = { clipWindow[j], clipWindow[(j + 1) % countOfwindow] };

			int ret = clipLL(boundary, clipWindow[(j + 2) % countOfwindow], line);

			if (ret == 0)		// ƽ������
				points.push_back(line[1]);
			else if (ret == 1)	// ���
			{
				points.push_back(line[0]);
				points.push_back(line[1]);
			}
			else if (ret == 2)	// ����
				points.push_back(line[1]);
		}
		res.clear();
		res.assign(points.begin(), points.end());
	}

	return res;
}

int main()
{
	std::vector<Point2> polygon = { {300,200},{500,200},{500,400},{300,400} };	//���ü������
	std::vector<Point2> clipWindow = { {300,200},{490,200},{450,390} };			//�ü�����

	auto result = clip(clipWindow, polygon);									//�ü�

	Canvas canvas(800, 600);													//������ͼ����
	fillPolygon(canvas, result);												//���ƶ����

	getchar();
}