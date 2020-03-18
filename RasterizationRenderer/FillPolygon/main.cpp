#include "Canvas.h"
#include "Point2.h"
#include <cstdio>
#include <vector>
#include <list>

// 多边形的边
struct Edge
{
	double x;			// 边的起点x坐标
	double dx;			// 扫描线每往上移动一行x的增量
	double ymax;		// 边的终点y坐标
	Edge(double x1, double dx1, double ymax1)
		: x(x1), dx(dx1), ymax(ymax1) {}
};
// 边排序函数
bool sortEdge(Edge const& edge1, Edge const& edge2)
{
	if (edge1.x != edge2.x)
		return edge1.x < edge2.x;
	else					
		return edge1.dx < edge2.dx;	// 如果x坐标相等，那么根据x坐标的增量来比较，增量越大则在下一行x坐标也越大
}

void fillPolygon(Canvas& canvas, std::vector<Point2>& points)
{
	if (points.size() < 3)
		return;

	// 创建NET(new edge table)表
	std::list<Edge>* NET = new std::list<Edge>[canvas.mScrHeight];

	int yMin = (int)points[0].y;			// 多边形y值的最小值
	int yMax = (int)points[0].y;			// 多边形y值的最大值
	// 初始化NET表
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

	// 创建AEL(active edge list)链表
	std::list<Edge> AEL;
	for (int y = yMin; y <= yMax; y++)
	{
		if (!NET[y].empty())								// 如果扫描行存在边，则加入AEL中
			AEL.splice(AEL.end(), NET[y]);
		AEL.sort(sortEdge);

		std::list<Edge>::iterator edgeStar, edgeEnd;		// 扫描行需要绘制的线段的端点的迭代器
		int countEdge = 0;									// 已存储的端点

		for (auto it = AEL.begin(); it != AEL.end(); )
		{
			// 扫描线已经到达某条边的y的最大值，则移除该边
			if ((int)it->ymax <= y)
			{
				it = AEL.erase(it);							// 这里记录移除元素的下一个元素的迭代器，不能用it++，因为it已经无效了。
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
					// 绘制线段
					for (int x = (int)edgeStar->x; x <= (int)edgeEnd->x; x++)
						canvas.setPixel(x, y, RED);
					// 这对交点的x值增加dx
					edgeStar->x += edgeStar->dx;
					edgeEnd->x += edgeEnd->dx;
				}
				it++;
			}
		}
	}
	delete[] NET;
}

int main()
{
	Canvas canvas(800, 600);
	std::vector<Point2> points{ 
		{400, 300}, {500, 300},
		{500, 200}, {400, 200} };

	fillPolygon(canvas, points);
	getchar();
	return 0;
}