/*
	�������ƣ�
			�ֵ�����(street flash)
	��飺
			ʹ��EasyXʵ���ڽֵ��Ͽղ�������

	ʵ��˼·��
			�������Ļ���˲��������������ӵ�·��ϸ��Ϊ��Σ�ÿһ�����ӵĳ���len�ͷ���angleͨ��rand()���ɣ�
			�����x����Ϊx += len * sin(angle)��y����Ϊy += len * cos(angle)���Ӷ�ģ������Ĳ�����
			ÿ��������������ͨ��ԭ������ÿ�����س���һ��ϵ����ʵ�֡�����һ���̵ܶ�ʱ��󣬻ָ�������ǰ�û��漴�ɡ�

	����:
			VS2017 + EasyX_20200109(beta)

	ʱ�䣺
			2020.2.23
	���ߣ�
			gaiyitp9

	�ο��� ���� ���䣺1479245050@qq.com  QQ�� 1479245050 
*/

#include <graphics.h>
#include <ctime>
#include <cmath>

const double PI			= 3.1415926;
const int brightness	= 5;				// ������ɫ����
const int screenWidth	= 640;
const int screenHeight	= 480;

//==============================
// �ֵ�������
//==============================
// ����
struct Window
{
	int width;
	int height;
};
// ��������1
struct House1
{
	int width;
	int height;
	Window win;
};
// ��������2
struct House2
{
	int width;
	int height;
	int top;
};

IMAGE temp;		// ���ڼ�¼����ǰ��ͼ��

int random(int index1, int index2);		// ����λ��[index2, index1 + index2)֮�䲻Ϊ0�������
void light(int x);						// ��������
void building();						// ���ƽֵ��ϵĽ���
int drawHouse1(int sumBorder);			// ���Ƶ�һ�ֽ���
int drawHouse2(int sumBorder);			// ���Ƶڶ��ֽ���
void lightChange();						// ���緢������ı仯

int main()
{
	srand((unsigned int)time(NULL));
	initgraph(screenWidth, screenHeight);

	setorigin(0, screenHeight);
	setaspectratio(1, -1);
	setbkcolor(RGB(20, 20, 20));
	cleardevice();

	building();

	int seedTime, sleepTime, lightX;

	while (true)
	{
		do {
			seedTime = rand() % 500;
		} while (seedTime <= 150);

		sleepTime = rand() % 3000 + seedTime;
		lightX = rand() % (screenWidth - 100) + 50;
		Sleep(sleepTime);
		light(lightX);
		lightChange();
	}

	closegraph();
	return 0;
}

// ����λ��[index2, index1 + index2)֮�䲻Ϊ0�������
int random(int index1, int index2)
{
	int num = 0;
	while (num == 0)
	{
		num = rand() % index1 + index2;
	}
	return num;
}
// ��������
void light(int x)
{
	int times = random(3, 0);			// ���������������һ��������
	int endPoint;						// ����������յ�
	for (int i = 0; i < times; i++)
	{
		int lightX = x, lightY = screenHeight;
		endPoint = lightY - rand() % (screenHeight / 2) - screenHeight / 4;
		while (lightY >= endPoint)
		{
			int len = rand() % 20;
			int angle = rand() % 120 - 60;
			int nextX = lightX + (int)(len * sin(angle * PI / 180));
			int nextY = lightY - (int)(len * cos(angle * PI / 180));
			setlinecolor(RGB(125, 250, 255));
			setlinestyle(PS_ENDCAP_ROUND | PS_SOLID, 2);
			line(lightX, lightY, nextX, nextY);
			lightX = nextX;
			lightY = nextY;
			Sleep((rand() % 100) > 66 ? 1 : 0);
		}
	}
}

// ���Ƶ�һ�ֽ���
int drawHouse1(int sumBorder)
{
	House1 house;
	house.width = random(40, 60);
	house.height = random(160, 80);
	house.win.width = 10;
	house.win.height = 8;
	int housePoints[8] = {
		sumBorder, 0,
		sumBorder, house.height,
		sumBorder + house.width, house.height,
		sumBorder + house.width, 0
	};
	setfillcolor(RGB(30, 30, 60));
	solidpolygon((POINT *)housePoints, 4);		// ���ƽ���ǽ��

	// ���ƽ����Ĵ���
	int winLeftDis = random(10, 18);
	int winTopDis = random(10, 10);
	int winInvDis = random(10, 10);
	for (int winY = house.height - winTopDis; winY > house.height / 3;)
	{
		for (int winX = sumBorder + winLeftDis; winX < sumBorder + house.width - 19;)
		{
			int winPoints[8] = {
				winX, winY,
				winX + house.win.width, winY,
				winX + house.win.width, winY - house.win.height,
				winX, winY - house.win.height
			};
			bool ranLight = random(3, 0) == 1 ? true : false; // ���ɿ��ƵĴ������ǹصƵĴ���
			if (ranLight)
				setfillcolor(RGB(240, 240, 150));
			else
				setfillcolor(RGB(30, 44, 40));

			solidpolygon((POINT *)winPoints, 4);
			winX += house.win.width + winInvDis;
		}
		winY -= house.win.height + winInvDis;
	}
	sumBorder += house.width;
	return sumBorder;
}

int drawHouse2(int sumBorder)
{
	House2 house;
	house.width = random(20, 80);
	house.height = random(40, 60);
	house.top = random(10, 100);
	int housePoints[10] = {
		sumBorder, 0,
		sumBorder, house.height,
		sumBorder + house.width / 2, house.top,
		sumBorder + house.width, house.height,
		sumBorder + house.width, 0
	};
	setfillcolor(RGB(30, 44, 83));
	solidpolygon((POINT *)housePoints, 5);			// ���ƽ���ǽ��

	// ���ƴ���
	bool circle = random(3, 0) == 1 ? true : false;
	if (circle)
	{
		bool ranLight = random(3, 0) == 1 ? true : false;
		if (ranLight)
			setfillcolor(RGB(150, 200, 130));
		else
			setfillcolor(RGB(30, 44, 50));

		int radius = random(10, house.height / 6);
		solidcircle(sumBorder + house.width / 2, house.height * 2 / 3, radius);
	}
	else
	{
		bool ranLight = random(3, 0) == 1 ? true : false;
		if (ranLight)
			setfillcolor(RGB(150, 200, 130));
		else
			setfillcolor(RGB(30, 44, 50));

		int radius = random(10, house.height / 6);
		solidcircle(sumBorder + house.width / 2, house.height * 3 / 5, radius);
		solidrectangle(sumBorder + house.width / 2 - radius, house.height * 3 / 5,
			sumBorder + house.width / 2 + radius, house.height * 3 / 5 - radius);
	}
	sumBorder += house.width;
	return sumBorder;
}

// ���ƽ�����
void building()
{
	int index = 0;
	for (int sumBorder = 0; sumBorder < screenWidth;)
	{
		index = random(3, 0);
		switch (index)
		{
		case 1:
			sumBorder = drawHouse1(sumBorder);
			break;
		case 2:
			sumBorder = drawHouse2(sumBorder);
			break;
		}
	}
	getimage(&temp, 0, 0, screenWidth, screenHeight);
}

// ���緢������ı仯
void lightChange()
{
	float brightness = (float)random(10, (random(10, 15) / 10));

	IMAGE image;
	getimage(&image, 0, 0, screenWidth, screenHeight);
	DWORD *pMem = GetImageBuffer(&image);
	int r, g, b;
	// ͼ����ÿһ�����ص���ɫ����������һ��ϵ��brightnessʹ����ɫ����
	for (int i = 0; i < (screenWidth * screenHeight); i++)
	{
		r = min((int)GetRValue(pMem[i] * brightness), 255);
		g = min((int)GetRValue(pMem[i] * brightness), 255);
		b = min((int)GetRValue(pMem[i] * brightness), 255);
		pMem[i] = RGB(r, g, b);
	}
	putimage(0, 0, &image);
	Sleep(100);				// �������ͼ�����100ms
	cleardevice();
	putimage(0, 0, &temp);	// �ָ�����ǰ��ͼ��
}