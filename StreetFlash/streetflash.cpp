/*
	程序名称：
			街道闪电(street flash)
	简介：
			使用EasyX实现在街道上空产生闪电

	实现思路：
			闪电从屏幕顶端产生，把闪电蔓延的路径细分为多段，每一次蔓延的长度len和方向angle通过rand()生成，
			闪电的x坐标为x += len * sin(angle)，y坐标为y += len * cos(angle)，从而模拟闪电的产生。
			每次闪电后画面变亮，通过原画面中每个像素乘以一个系数来实现。持续一个很短的时间后，恢复到闪电前得画面即可。

	环境:
			VS2017 + EasyX_20200109(beta)

	时间：
			2020.2.23
	作者：
			gaiyitp9

	参考自 星羽 邮箱：1479245050@qq.com  QQ： 1479245050 
*/

#include <graphics.h>
#include <ctime>
#include <cmath>

const double PI			= 3.1415926;
const int brightness	= 5;				// 闪电颜色亮度
const int screenWidth	= 640;
const int screenHeight	= 480;

//==============================
// 街道建筑物
//==============================
// 窗户
struct Window
{
	int width;
	int height;
};
// 房子类型1
struct House1
{
	int width;
	int height;
	Window win;
};
// 房子类型2
struct House2
{
	int width;
	int height;
	int top;
};

IMAGE temp;		// 用于记录闪电前的图像

int random(int index1, int index2);		// 产生位于[index2, index1 + index2)之间不为0得随机数
void light(int x);						// 产生闪电
void building();						// 绘制街道上的建筑
int drawHouse1(int sumBorder);			// 绘制第一种建筑
int drawHouse2(int sumBorder);			// 绘制第二种建筑
void lightChange();						// 闪电发生后画面的变化

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

// 产生位于[index2, index1 + index2)之间不为0得随机数
int random(int index1, int index2)
{
	int num = 0;
	while (num == 0)
	{
		num = rand() % index1 + index2;
	}
	return num;
}
// 产生闪电
void light(int x)
{
	int times = random(3, 0);			// 产生闪电的数量，一条或两条
	int endPoint;						// 闪电延伸的终点
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

// 绘制第一种建筑
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
	solidpolygon((POINT *)housePoints, 4);		// 绘制建筑墙身

	// 绘制建筑的窗户
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
			bool ranLight = random(3, 0) == 1 ? true : false; // 生成开灯的窗户还是关灯的窗户
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
	solidpolygon((POINT *)housePoints, 5);			// 绘制建筑墙身

	// 绘制窗户
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

// 绘制建筑物
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

// 闪电发生后画面的变化
void lightChange()
{
	float brightness = (float)random(10, (random(10, 15) / 10));

	IMAGE image;
	getimage(&image, 0, 0, screenWidth, screenHeight);
	DWORD *pMem = GetImageBuffer(&image);
	int r, g, b;
	// 图像中每一个像素的颜色分量都乘以一个系数brightness使得颜色变亮
	for (int i = 0; i < (screenWidth * screenHeight); i++)
	{
		r = min((int)GetRValue(pMem[i] * brightness), 255);
		g = min((int)GetRValue(pMem[i] * brightness), 255);
		b = min((int)GetRValue(pMem[i] * brightness), 255);
		pMem[i] = RGB(r, g, b);
	}
	putimage(0, 0, &image);
	Sleep(100);				// 变亮后的图像持续100ms
	cleardevice();
	putimage(0, 0, &temp);	// 恢复到以前的图像
}