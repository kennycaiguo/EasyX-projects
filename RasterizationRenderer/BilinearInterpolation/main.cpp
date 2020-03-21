#include "Canvas.h"
#include "Point2.h"
#include "Vector3.h"
#include <cstdio>
#include <vector>
#include <array>
#include <list>
#include <chrono>

// ����εı�
struct Edge
{
	double x;			// �ߵ����x����
	double dx;			// ɨ����ÿ�����ƶ�һ��x������
	double ymax;		// �ߵ��յ�y����
	double u, v;		// ��������
	double du, dv;		// ɨ����ÿ�����ƶ�һ���������������
	Edge(double x1, double dx1, double ymax1, double u1, double v1, double du1, double dv1)
		: x(x1), dx(dx1), ymax(ymax1), u(u1), v(v1), du(du1), dv(dv1) {}
};
// ��������
bool sortEdge(Edge const& edge1, Edge const& edge2)
{
	if (edge1.x != edge2.x)
		return edge1.x < edge2.x;
	else					
		return edge1.dx < edge2.dx;	// ���x������ȣ���ô����x������������Ƚϣ�����Խ��������һ��x����ҲԽ��
}
// ����2x2��С��ͼƬ
COLORREF picture[2][2] = {
	{RGB(255, 0, 0), RGB(255, 255, 255)},
	{RGB(0, 255, 0), RGB(0, 0, 255)}
};
// ��ȡͼƬ��ɫ
COLORREF getPixel(int x, int y)
{
	return picture[x][y];
}
// ����������ת��ΪͼƬ��xy����
void UV2XY(double u, double v, double& x, double& y, double w, double h)
{
	x = u * w;
	y = v * h;
	x = min(max(x, 0), w);	// ��x������0-w
	y = min(max(y, 0), h);	// ��y������0-h
}

// ������������
void fillTriangle(Canvas& canvas, std::array<Point2, 3>& points, double* coordinate)
{
	Vector3 ab(points[1].x - points[0].x, points[1].y - points[0].y, 0.0);
	Vector3 bc(points[2].x - points[1].x, points[2].y - points[1].y, 0.0);
	Vector3 ac(points[2].x - points[0].x, points[2].y - points[0].y, 0.0);
	Vector3 ca(points[0].x - points[2].x, points[0].y - points[2].y, 0.0);
	double area = cross(ab, ac).length();	// ���������������
	if (area == 0)
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
			double u = coordinate[(i + 1) % count * 2];
			double v = coordinate[(i + 1) % count * 2 + 1];
			double du = (coordinate[(i + 1) % count * 2] - coordinate[i % count * 2]) 
				/ (points[(i + 1) % count].y - points[i].y);
			double dv = (coordinate[(i + 1) % count * 2 + 1] - coordinate[i % count * 2 + 1])
				/ (points[(i + 1) % count].y - points[i].y);
			NET[(int)points[(i + 1) % count].y].push_back(Edge(x, dx, ymax, u, v, du, dv));
		}
		else if (points[i].y < points[(i + 1) % count].y)
		{
			double x = points[i].x;
			double dx = (points[(i + 1) % count].x - points[i].x)
				/ (points[(i + 1) % count].y - points[i].y);
			double ymax = points[(i + 1) % count].y;
			double u = coordinate[i % count * 2];
			double v = coordinate[i % count * 2 + 1];
			double du = (coordinate[(i + 1) % count * 2] - coordinate[i % count * 2])
				/ (points[(i + 1) % count].y - points[i].y);
			double dv = (coordinate[(i + 1) % count * 2 + 1] - coordinate[i % count * 2 + 1])
				/ (points[(i + 1) % count].y - points[i].y);
			NET[(int)points[i].y].push_back(Edge(x, dx, ymax, u, v, du, dv));
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

					double su = edgeStar->u;
					double eu = edgeEnd->u;
					double sv = edgeStar->v;
					double ev = edgeEnd->v;
					double sx = edgeStar->x;
					double ex = edgeEnd->x;

					double dxu = (su - eu) / (sx - ex);
					double dxv = (sv - ev) / (sx - ex);

					double u = su;
					double v = sv;
					// �����߶�
					for (int x = (int)sx; x <= (int)ex; x++)
					{
						double px, py;				// ͼƬ������
						UV2XY(u, v, px, py, 2, 2);
						COLORREF c = getPixel((int)px, (int)py);
						canvas.setPixel(x, y, c);	// ��������
						u += dxu;
						v += dxv;
					}
					// ��Խ����xֵ����dx
					edgeStar->x += edgeStar->dx;
					edgeEnd->x += edgeEnd->dx;
					edgeStar->u += edgeStar->du;
					edgeEnd->u += edgeEnd->du;
					edgeStar->v += edgeStar->dv;
					edgeEnd->v += edgeEnd->dv;
				}
				it++;
			}
		}
	}
	delete[] NET;
}

int main()
{
	std::array<Point2, 3> triangle = { Point2(400, 300), Point2(600, 300), Point2(600, 500) };
	//��������
	double coordinate[] = {
		0,0,1,0,1,1
	};
	//������ͼ����
	Canvas canvas(800, 600);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	//������������
	fillTriangle(canvas, triangle, coordinate);	

	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	//����ʱ��(��)
	double useTime = double(duration.count()) * 
		std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	//����������ӡ��Ϣ�õĻ�����
	char msg[256];
	sprintf_s(msg, 256, "Debug�汾��˫���Բ�ֵ���ƺ�ʱ:%lf ����\n", useTime * 1000);
	//�������������֡����ʱ����
	OutputDebugStringA(msg);

	getchar();
	return 0;
}