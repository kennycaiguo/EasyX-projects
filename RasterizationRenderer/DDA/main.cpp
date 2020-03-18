#include "Canvas.h"
#include "Point2.h"
#include <cstdio>
#include <cstdlib>

// 微分分析法(DDA)绘制直线
void DDA(Canvas& canvas, Point2& A, Point2& B)
{
	// 如果A.X-B.X==0的话，使用int会出问题，会报除0异常，但是double有inf(infinite)和nan(not a number)，
	// 并且1/0=inf,1/inf=0，inf大于任何有效数字，-inf小于任何有效数字，所以这里可以不管分母为0的情况
	double k = (B.y - A.y) / (B.x - A.x);

	// 斜率小于1，按照x步进
	if (abs(k) < 1)
	{
		// 保证A的x坐标比B的x坐标小
		if (A.x > B.x)
		{
			Point2 temp = A;
			A = B;
			B = temp;
		}

		for (double x = A.x, y = A.y; x <= B.x; x++, y+=k)
			canvas.setPixel((int)x, (int)y, WHITE);
	}
	// 斜率大于等于1，按照y步进
	else
	{
		// 保证A的y坐标比B的y坐标小
		if (A.y > B.y)
		{
			Point2 temp = A;
			A = B;
			B = temp;
		}

		for (double x = A.x, y = A.y; y <= B.y; y++, x+=1.0/k)
			canvas.setPixel((int)x, (int)y, WHITE);
	}
}

int main()
{
	Canvas canvas(800, 600);			// 创建画布				
	Point2 points[2] = {				// 指定线段的端点
		{500, 400}, {300, 200}
	};
	DDA(canvas, points[0], points[1]);	// 绘制直线

	getchar();
	return 0;
}