/*
	程序名称：
			流体(Fluid)
	简介：
			使用EasyX实现流体效果。
			模拟的流体会向鼠标中心聚集，移动鼠标流体会跟着鼠标运动。当点击鼠标左键后，流体会散开。

	实现思路：
			流体由一定数量的粒子组成，每个粒子受到指向鼠标中心的引力而向鼠标中心运动。当鼠标移动时，
			粒子会受到一个沿着鼠标方向移动的搅动力，表现出粒子收到鼠标搅动而运动。此外，当点击鼠标
			时，粒子受到一个沿着鼠标中心向外的力，产生爆炸的效果。实现流体有两个关键：
			1. 绘制每个粒子的运动轨迹。这通过一个使屏幕变暗的函数实现，每一帧先将之前的图像变暗，
			   然后再绘制粒子的新位置，这样就使得绘制在之前位置的粒子慢慢消失。
			2. 更新每个粒子的位置。这通过牛顿第二定律进行近似计算。当粒子碰到屏幕边缘时，粒子被反弹，
			   速度与碰撞前相反。

	环境：
			VS2019 + EasyX_20200109(beta)

	时间：
			2020.3.12

	作者：
			gaiyitp9

	参考自krissi，原地址https://codebus.cn/zhaoh/post/liquid-particles
*/

#include <graphics.h>
#include <cmath>
#include "Timer.h"
using namespace std;

//===========================================
// Fluid 模拟流体
//===========================================
class Fluid
{
private:
	const int mScreenWidth = 600;			// 应用程序屏幕宽度
	const int mScreenHeight = 400;			// 应用程序屏幕高度
	enum { NumOfParticles = 600 };			// 粒子数

	struct Particle
	{
		COLORREF color;		// 粒子颜色
		float x, y;			// 粒子坐标
		float vX, vY;		// 粒子速度
	};

	Timer		mTimer;						// 计时器

	Particle*	mParticles;					// 模拟流体中粒子数

	POINT		mMouse;						// 当前帧鼠标位置
	POINT		mPrevMouse;					// 前一帧鼠标位置
	bool		mIsLButtonDown;				// 鼠标左键是否按下

	DWORD*		pMem;						// 显存指针

	float		mAttractDist	= mScreenWidth * 0.86f;		// 引力范围
	float		mBlowDist		= mScreenWidth * 0.5f;		// 爆炸范围
	float		mStirDist		= mScreenWidth * 0.125f;	// 搅动范围
	float		mFriction		= 0.96f;					// 摩擦力

public:
	Fluid();
	~Fluid() { closegraph(); }

	void Simulate();			// 绘制模拟流体

private:
	void Darken();			// 全屏变暗50%
	void Move();			// 模拟流体运动
};

Fluid::Fluid()
{
	mMouse.x = mScreenWidth / 2;				// 鼠标初始位置在屏幕中心
	mMouse.y = mScreenHeight / 2;
	mPrevMouse.x = mScreenWidth / 2;
	mPrevMouse.y = mScreenHeight / 2;
	mIsLButtonDown = false;

	srand((unsigned int)time(0));				// 设置随机数种子
	// 创建绘图窗口
	initgraph(mScreenWidth, mScreenHeight);
	pMem = GetImageBuffer();					// 获取显存指针

	mParticles = new Particle[NumOfParticles];	// 初始化模拟流体
	for (int i = 0; i < NumOfParticles; i++)
	{
		mParticles[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		mParticles[i].x = mScreenWidth / 2.0f;
		mParticles[i].y = mScreenHeight / 2.0f;
		mParticles[i].vX = (float)(cos(float(i))) * (rand() % 34);
		mParticles[i].vY = (float)(sin(float(i))) * (rand() % 34);
	}
}

void Fluid::Simulate()
{
	// 批量绘图
	BeginBatchDraw();

	while (true)
	{
		static double elapsed = 0.0;
		mTimer.Update();	// 更新计时器
		if (mTimer.TotalTime() - elapsed >= 0.02)	// 每20 ms刷新一次
		{
			elapsed = mTimer.TotalTime();
			mTimer.Tick();
			// 处理鼠标消息
			while (MouseHit())
			{
				MOUSEMSG msg = GetMouseMsg();
				switch (msg.uMsg)
				{
				case WM_MOUSEMOVE:		mMouse.x = msg.x; mMouse.y = msg.y;	break;
				case WM_LBUTTONDOWN:	mIsLButtonDown = true;				break;
				case WM_LBUTTONUP:		mIsLButtonDown = false;				break;
				}
			}
			Darken();			// 全屏变暗，使得之前绘制的小球慢慢消失，从而模拟出小球运动轨迹
			Move();				// 更新粒子的位置
			FlushBatchDraw();
		}
	}
	// 结束批量绘图
	EndBatchDraw();
}

void Fluid::Darken()
{
	for (int i = 0; i < mScreenWidth * mScreenHeight; i++)
		if (pMem[i] != 0)
			pMem[i] = RGB(GetRValue(pMem[i]) >> 1, GetGValue(pMem[i]) >> 1, GetBValue(pMem[i]) >> 1);
}

void Fluid::Move()
{
	int mouseVX = mMouse.x - mPrevMouse.x;		// 鼠标在x轴的速度
	int mouseVY = mMouse.y - mPrevMouse.y;		// 鼠标在y轴的速度
	mPrevMouse.x = mMouse.x;
	mPrevMouse.y = mMouse.y;
	
	for (int i = 0; i < NumOfParticles; i++)
	{
		float dX = mParticles[i].x - mMouse.x;
		float dY = mParticles[i].y - mMouse.y;
		float d = sqrt(dX * dX + dY * dY);
		dX = d ? dX / d : 0;
		dY = d ? dY / d : 0;

		// 如果按下了鼠标左键且粒子如果在爆炸范围内，偏离鼠标中心运动
		if (mIsLButtonDown && d < mBlowDist)
		{
			float blowAcc = (1 - (d / mBlowDist)) * 14;
			mParticles[i].vX += blowAcc * dX + 0.5f - float(rand()) / RAND_MAX;
			mParticles[i].vY += blowAcc * dY + 0.5f - float(rand()) / RAND_MAX;
		}
		// 如果粒子在引力范围内，向鼠标中心运动
		if (d < mAttractDist)
		{
			float AttractAcc = (1 - (d / mAttractDist)) * mScreenWidth * 0.0014f;
			mParticles[i].vX -= dX * AttractAcc;
			mParticles[i].vY -= dY * AttractAcc;
		}
		// 如果粒子在搅动范围内，则沿着鼠标移动的方向移动
		if (d < mStirDist)
		{
			float StirAcc = (1 - (d / mStirDist)) * mScreenWidth * 0.00026f;
			mParticles[i].vX += mouseVX * StirAcc;
			mParticles[i].vY += mouseVY * StirAcc;
		}
		// 粒子速度乘以摩擦系数
		mParticles[i].vX *= mFriction;
		mParticles[i].vY *= mFriction;

		float avgVX = (float)fabs(mParticles[i].vX);
		float avgVY = (float)fabs(mParticles[i].vY);
		float avgV = (avgVX + avgVY) * 0.5f;
		// 粒子速度如果小于0.1，则速度乘以一个[0, 3]的系数，保证粒子速度不为0
		if (avgVX < 0.1) mParticles[i].vX *= float(rand()) / RAND_MAX * 3;
		if (avgVY < 0.1) mParticles[i].vY *= float(rand()) / RAND_MAX * 3;
		// 粒子的半径，限制在[0.9, 4.0]之间
		float radius = avgV * 0.45f;
		radius = max(min(radius, 4.0f), 0.9f);

		float nextX = mParticles[i].x + mParticles[i].vX;
		float nextY = mParticles[i].y + mParticles[i].vY;
		// 如果粒子碰到了屏幕边缘，则发生反弹
		if (nextX > mScreenWidth) { nextX = (float)mScreenWidth;	mParticles[i].vX *= -1; }
		else if (nextX < 0) { nextX = 0;		mParticles[i].vX *= -1; }
		if (nextY > mScreenHeight) { nextY = (float)mScreenHeight;	mParticles[i].vY *= -1; }
		else if (nextY < 0) { nextY = 0;		mParticles[i].vY *= -1; }

		mParticles[i].x = nextX;
		mParticles[i].y = nextY;

		// 绘制粒子
		setcolor(mParticles[i].color);
		setfillstyle(BS_SOLID);
		fillcircle(int(mParticles[i].x), int(mParticles[i].y), int(radius));
	}
}

int main()
{
	Fluid fluid;
	fluid.Simulate();

	return 0;
}