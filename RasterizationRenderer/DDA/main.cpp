#include "Canvas.h"
#include "Point2.h"
#include <cstdio>
#include <cstdlib>

// ΢�ַ�����(DDA)����ֱ��
void DDA(Canvas& canvas, Point2& A, Point2& B)
{
	// ���A.X-B.X==0�Ļ���ʹ��int������⣬�ᱨ��0�쳣������double��inf(infinite)��nan(not a number)��
	// ����1/0=inf,1/inf=0��inf�����κ���Ч���֣�-infС���κ���Ч���֣�����������Բ��ܷ�ĸΪ0�����
	double k = (B.y - A.y) / (B.x - A.x);

	// б��С��1������x����
	if (abs(k) < 1)
	{
		// ��֤A��x�����B��x����С
		if (A.x > B.x)
		{
			Point2 temp = A;
			A = B;
			B = temp;
		}

		for (double x = A.x, y = A.y; x <= B.x; x++, y+=k)
			canvas.setPixel((int)x, (int)y, WHITE);
	}
	// б�ʴ��ڵ���1������y����
	else
	{
		// ��֤A��y�����B��y����С
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
	Canvas canvas(800, 600);			// ��������				
	Point2 points[2] = {				// ָ���߶εĶ˵�
		{500, 400}, {300, 200}
	};
	DDA(canvas, points[0], points[1]);	// ����ֱ��

	getchar();
	return 0;
}