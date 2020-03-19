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
						canvas.setPixel(x, y, WHITE);
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

/*
使用直线裁剪一条直线
对于一个凸多边形来说，他所有的顶点都在任意一条边的同一侧，所以当使用该多边形的某一条边作为裁剪边界，则选用任意不在这条边上面的顶点r可作为裁剪参照条件
当被裁剪顶点p和r不在直线的同一侧时说明p会被裁剪掉
本函数如果裁剪了Line,会修改传入参数line的坐标，修改A点时返回1,修改B点时返回2，平凡接受时返回0，平凡拒绝返回-1
裁剪直线的端点分别为Boundary0和Boundary1。被裁剪直线为line，存有两个端点
*/
int clipLL(std::vector<Point2>& boundary, Point2& reference, std::vector<Point2>& line)
{
	// 裁剪直线的一般式系数
	double A1 = boundary[1].y - boundary[0].y;
	double B1 = boundary[0].x - boundary[1].x;
	double C1 = boundary[1].x * boundary[0].y - boundary[0].x * boundary[1].y;

	// 被裁剪直线端点是否被裁剪
	bool clipA = false , clipB = false;

	if ((A1 * reference.x + B1 * reference.y + C1) * (A1 * line[0].x + B1 * line[0].y + C1) < 0)
		clipA = true;
	if ((A1 * reference.x + B1 * reference.y + C1) * (A1 * line[1].x + B1 * line[1].y + C1) < 0)
		clipB = true;

	if (!clipA && !clipB)		// 平凡接受
		return 0;
	else if (clipA && clipB)	// 平凡拒绝
		return -1;

	// 被裁剪直线的一般式系数
	double A2 = line[1].y - line[0].y;
	double B2 = line[0].x - line[1].x;
	double C2 = line[1].x * line[0].y - line[0].x * line[1].y;
	// 裁剪直线与被裁剪直线的交点
	double x = (C2 * B1 - C1 * B2) / (A1 * B2 - A2 * B1);
	double y = (C1 * A2 - C2 * A1) / (A1 * B2 - A2 * B1);

	if (clipA)	
	{
		line[0].x = x;	// 入点
		line[0].y = y;
		return 1;
	}
	else
	{
		line[1].x = x;	// 出点
		line[1].y = y;
		return 2;
	}
}

/*
使用一个凸多边形作为裁剪窗口
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

			if (ret == 0)		// 平凡接受
				points.push_back(line[1]);
			else if (ret == 1)	// 入点
			{
				points.push_back(line[0]);
				points.push_back(line[1]);
			}
			else if (ret == 2)	// 出点
				points.push_back(line[1]);
		}
		res.clear();
		res.assign(points.begin(), points.end());
	}

	return res;
}

int main()
{
	std::vector<Point2> polygon = { {300,200},{500,200},{500,400},{300,400} };	//被裁剪多边形
	std::vector<Point2> clipWindow = { {300,200},{490,200},{450,390} };			//裁剪窗口

	auto result = clip(clipWindow, polygon);									//裁剪

	Canvas canvas(800, 600);													//创建绘图窗口
	fillPolygon(canvas, result);												//绘制多边形

	getchar();
}