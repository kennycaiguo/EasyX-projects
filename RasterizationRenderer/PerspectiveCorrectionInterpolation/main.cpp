#include "Canvas.h"
#include "Point2.h"
#include "Point3.h"
#include "Point4.h"
#include "Vector3.h"
#include <cstdio>
#include <vector>
#include <array>
#include <list>
#include <chrono>

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
// 构造2x2大小的图片
COLORREF picture[2][2] = {
	{RGB(255, 0, 0), RGB(255, 255, 255)},
	{RGB(0, 255, 0), RGB(0, 0, 255)}
};
// 获取图片颜色
COLORREF getPixel(int x, int y)
{
	return picture[x][y];
}
// 把纹理坐标转换为图片的xy坐标
void UV2XY(double u, double v, double& x, double& y, double w, double h)
{
	x = u * w;
	y = v * h;
	x = min(max(x, 0), w);	// 把x限制在0-w
	y = min(max(y, 0), h);	// 把y限制在0-h
}

void fillTriangle(Canvas& canvas, std::vector<Point4>& triangle, std::vector<Point2>& coordinates)
{
	std::vector<Point4> points(3);
	for (int i = 0; i < 3; i++)
	{
		points[i].x = (triangle[i].x + 1) * 0.5 * ((double)canvas.mScrWidth - 1);
		points[i].y = (triangle[i].y + 1) * 0.5 * ((double)canvas.mScrHeight - 1);
		points[i].w = triangle[i].w;
	}

	Vector3 ab(points[1].x - points[0].x, points[1].y - points[0].y, 0.0);
	Vector3 bc(points[2].x - points[1].x, points[2].y - points[1].y, 0.0);
	Vector3 ac(points[2].x - points[0].x, points[2].y - points[0].y, 0.0);
	Vector3 ca(points[0].x - points[2].x, points[0].y - points[2].y, 0.0);
	double area = cross(ab, ac).length();	// 三角形面积的两倍
	if (area == 0)
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
					{
						double weightA, weightB, weightC;
						Vector3 ap(x - points[0].x, y - points[0].y, 0.0);
						Vector3 bp(x - points[1].x, y - points[1].y, 0.0);
						Vector3 cp(x - points[2].x, y - points[2].y, 0.0);
						weightA = cross(bc, bp).length() / area;
						weightB = cross(ca, cp).length() / area;
						weightC = cross(ab, ap).length() / area;
						
						double omega = 1.0 / ((1.0 / points[0].w) * weightA + (1.0 / points[1].w) * weightB + 
							(1.0 / points[2].w) * weightC);
						double u = omega * ((coordinates[0].u / points[0].w) * weightA + (coordinates[1].u / points[1].w) * weightB + 
							(coordinates[2].u / points[2].w) * weightC);
						double v = omega * ((coordinates[0].v / points[0].w) * weightA + (coordinates[1].v / points[1].w) * weightB +
							(coordinates[2].v / points[2].w) * weightC);

						double px, py;
						UV2XY(u, v, px, py, 2, 2);
						COLORREF color = getPixel((int)px, (int)py);
						canvas.setPixel(x, y, color);
					}
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

// 投影函数,分别传递一个点和near平面，计算出新投影点并将结果保存至一个齐次坐标中
void projection(Point3& point, double l, double r, double b, double t, double n, double f, Point4& result)
{
	result.x = (2 * n * point.x) / (r - l) - point.z * (r + l) / (r - l);
	result.y = (2 * n * point.y) / (t - b) - point.z * (t + b) / (t - b);
	result.z = (f + n) / (f - n) * point.z + 2 * f * n / (n - f);
	result.w = point.z;
}

//对直线进行裁剪,返回-1表示平凡拒绝，返回0表示平凡接受，返回1表示对A点进行裁剪，返回2表示对B点进行裁剪，最后两个参数result和t表示裁剪点坐标和t值
//其中参数flag从0到5分别表示边界为left、right、bottom、top、near、far
int clipLine(int flag, Point4& A, Point4& B, Point4& result, double& t)
{
	double aClipCondition = 0;
	double bClipCondition = 0;
	switch (flag)
	{
	case 0:
		aClipCondition = A.x + A.w;
		bClipCondition = B.x + B.w;
		break;
	case 1:
		aClipCondition = A.w - A.x;
		bClipCondition = B.w - B.x;
		break;
	case 2:
		aClipCondition = A.y + A.w;
		bClipCondition = B.y + B.w;
		break;
	case 3:
		aClipCondition = A.w - A.y;
		bClipCondition = B.w - B.y;
		break;
	case 4:
		aClipCondition = A.z + A.w;
		bClipCondition = B.z + B.w;
		break;
	case 5:
		aClipCondition = A.w - A.z;
		bClipCondition = B.z - B.z;
		break;
	default:
		return 0;
	}
	if (aClipCondition < 0 && bClipCondition < 0)
		return -1;
	if (aClipCondition >= 0 && bClipCondition >= 0)
		return 0;

	t = aClipCondition / (aClipCondition - bClipCondition);
	result.x = A.x + (B.x - A.x) * t;
	result.y = A.y + (B.y - A.y) * t;
	result.z = A.z + (B.z - A.z) * t;
	result.w = A.w + (B.w - A.w) * t;
	if (aClipCondition < 0)
		return 1;
	else
		return 2;
}

void clipTriangleAndDraw(std::vector<Point4>& triangle, std::vector<double>& attribute, Canvas& canvas)
{
	std::vector<Point4> points(triangle.begin(), triangle.end());	// 裁剪后顶点数量
	std::vector<double> attributes(attribute);						// 裁剪后顶点属性
	
	// 用视椎体对三角形进行裁剪，裁剪方法与Suthland-Hodgman算法类似
	for (int clipflag = 0; clipflag < 6; clipflag++)
	{
		std::vector<Point4> temp;		// 用于储存本次裁剪之后的顶点
		std::vector<double> tempAttri;	// 用于储存本次裁剪之后的顶点属性
		int size = points.size();
		for (int i = 0; i < size; i++)
		{
			Point4 result;				// 边界与三角形边的交点
			double t;					// 交点线性插值的系数
			size_t indexA = i, indexB = (i + 1) % size;
			int ret = clipLine(clipflag, points[indexA], points[indexB], result, t);
			// 平凡接受
			if (ret == 0)	
			{
				temp.push_back(points[indexB]);
				tempAttri.push_back(attributes[indexB * 2]);
				tempAttri.push_back(attributes[indexB * 2 + 1]);
			}
			// 裁剪A点，加入入点和B点
			else if (ret == 1)
			{
				temp.push_back(result);
				tempAttri.push_back(attributes[indexA * 2] + (attributes[indexB * 2] - attributes[indexA * 2]) * t);
				tempAttri.push_back(attributes[indexA * 2 + 1] + (attributes[indexB * 2 + 1] - attributes[indexA * 2 + 1]) * t);
				temp.push_back(points[(i + 1) % size]);
				tempAttri.push_back(attributes[indexB * 2]);
				tempAttri.push_back(attributes[indexB * 2 + 1]);
			}
			// 裁剪B点，加入出点
			else if (ret == 2)
			{
				temp.push_back(result);
				tempAttri.push_back(attributes[indexA * 2] + (attributes[indexB * 2] - attributes[indexA * 2]) * t);
				tempAttri.push_back(attributes[indexA * 2 + 1] + (attributes[indexB * 2 + 1] - attributes[indexA * 2 + 1]) * t);
			}
		}
		points.clear();
		points.assign(temp.begin(), temp.end());
		attributes.clear();
		attributes.assign(tempAttri.begin(), tempAttri.end());
	}
	// 将裁剪后的图形分解成多个三角形进行绘制，记得要先进行透视除法将坐标变换到CVV空间
	std::vector<Point4> out(3);
	std::vector<Point2> coordinates(3);
	for (size_t i = 1; i < points.size() - 1; i++)
	{
		out[0].x = points[0].x / points[0].w;
		out[0].y = points[0].y / points[0].w;
		out[0].w = points[0].w;
		out[1].x = points[i].x / points[i].w;
		out[1].y = points[i].y / points[i].w;
		out[1].w = points[i].w;
		out[2].x = points[i+1].x / points[i+1].w;
		out[2].y = points[i+1].y / points[i+1].w;
		out[2].w = points[i+1].w;

		coordinates[0].u = attributes[0];
		coordinates[0].v = attributes[1];
		coordinates[1].u = attributes[i * 2];
		coordinates[1].v = attributes[i * 2 + 1];
		coordinates[2].u = attributes[(i + 1) * 2];
		coordinates[2].v = attributes[(i + 1) * 2 + 1];

		fillTriangle(canvas, out, coordinates);
	}
}

int main()
{
	//创建绘图窗口
	Canvas canvas(640, 480);

	std::vector<Point3> triangle1 = { 
		{-639.90252489721047,-480,88.830459715149615},
		{-639.90252489721047,480,88.830459715149615},
		{639.90252489721047,-480,111.16954028485038} 
	};
	std::vector<double> coordinates1 = { 0, 0, 0, 1, 1, 0 };
	std::vector<Point3> triangle2 = { 
		{639.90252489721047,480,111.16954028485038},
		{639.90252489721047,-480,111.16954028485038},
		{-639.90252489721047,480,88.830459715149615} 
	};
	std::vector<double> coordinates2 = { 1, 1, 1, 0, 0, 1 };

	// 裁剪空间坐标
	std::vector<Point4> projection1(3);
	std::vector<Point4> projection2(3);
	for (int i = 0; i < 3; i++)
	{
		projection(triangle1[i], -320, 320, -240, 240, 25, 200, projection1[i]);
		projection(triangle2[i], -320, 320, -240, 240, 25, 200, projection2[i]);
	}

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	
	// 裁剪三角形并绘制
	clipTriangleAndDraw(projection1, coordinates1, canvas);
	clipTriangleAndDraw(projection2, coordinates2, canvas);

	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	//花费时间(秒)
	double useTime = double(duration.count()) * 
		std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	//往调试器打印信息用的缓冲区
	char msg[256];
	sprintf_s(msg, 256, "Debug版本，重心坐标插值绘制耗时:%lf 毫秒\n", useTime * 1000);
	//往调试器输出两帧绘制时间间隔
	OutputDebugStringA(msg);

	getchar();
	return 0;
}