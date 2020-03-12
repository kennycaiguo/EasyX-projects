/*
	�������ƣ�
			����(Fluid)
	��飺
			ʹ��EasyXʵ������Ч����
			ģ����������������ľۼ����ƶ����������������˶���������������������ɢ����

	ʵ��˼·��
			������һ��������������ɣ�ÿ�������ܵ�ָ��������ĵ�����������������˶���������ƶ�ʱ��
			���ӻ��ܵ�һ��������귽���ƶ��Ľ����������ֳ������յ����������˶������⣬��������
			ʱ�������ܵ�һ����������������������������ը��Ч����ʵ�������������ؼ���
			1. ����ÿ�����ӵ��˶��켣����ͨ��һ��ʹ��Ļ�䰵�ĺ���ʵ�֣�ÿһ֡�Ƚ�֮ǰ��ͼ��䰵��
			   Ȼ���ٻ������ӵ���λ�ã�������ʹ�û�����֮ǰλ�õ�����������ʧ��
			2. ����ÿ�����ӵ�λ�á���ͨ��ţ�ٵڶ����ɽ��н��Ƽ��㡣������������Ļ��Եʱ�����ӱ�������
			   �ٶ�����ײǰ�෴��

	������
			VS2019 + EasyX_20200109(beta)

	ʱ�䣺
			2020.3.12

	���ߣ�
			gaiyitp9

	�ο���krissi��ԭ��ַhttps://codebus.cn/zhaoh/post/liquid-particles
*/

#include <graphics.h>
#include <cmath>
#include "Timer.h"
using namespace std;

//===========================================
// Fluid ģ������
//===========================================
class Fluid
{
private:
	const int mScreenWidth = 600;			// Ӧ�ó�����Ļ���
	const int mScreenHeight = 400;			// Ӧ�ó�����Ļ�߶�
	enum { NumOfParticles = 600 };			// ������

	struct Particle
	{
		COLORREF color;		// ������ɫ
		float x, y;			// ��������
		float vX, vY;		// �����ٶ�
	};

	Timer		mTimer;						// ��ʱ��

	Particle*	mParticles;					// ģ��������������

	POINT		mMouse;						// ��ǰ֡���λ��
	POINT		mPrevMouse;					// ǰһ֡���λ��
	bool		mIsLButtonDown;				// �������Ƿ���

	DWORD*		pMem;						// �Դ�ָ��

	float		mAttractDist	= mScreenWidth * 0.86f;		// ������Χ
	float		mBlowDist		= mScreenWidth * 0.5f;		// ��ը��Χ
	float		mStirDist		= mScreenWidth * 0.125f;	// ������Χ
	float		mFriction		= 0.96f;					// Ħ����

public:
	Fluid();
	~Fluid() { closegraph(); }

	void Simulate();			// ����ģ������

private:
	void Darken();			// ȫ���䰵50%
	void Move();			// ģ�������˶�
};

Fluid::Fluid()
{
	mMouse.x = mScreenWidth / 2;				// ����ʼλ������Ļ����
	mMouse.y = mScreenHeight / 2;
	mPrevMouse.x = mScreenWidth / 2;
	mPrevMouse.y = mScreenHeight / 2;
	mIsLButtonDown = false;

	srand((unsigned int)time(0));				// �������������
	// ������ͼ����
	initgraph(mScreenWidth, mScreenHeight);
	pMem = GetImageBuffer();					// ��ȡ�Դ�ָ��

	mParticles = new Particle[NumOfParticles];	// ��ʼ��ģ������
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
	// ������ͼ
	BeginBatchDraw();

	while (true)
	{
		static double elapsed = 0.0;
		mTimer.Update();	// ���¼�ʱ��
		if (mTimer.TotalTime() - elapsed >= 0.02)	// ÿ20 msˢ��һ��
		{
			elapsed = mTimer.TotalTime();
			mTimer.Tick();
			// ���������Ϣ
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
			Darken();			// ȫ���䰵��ʹ��֮ǰ���Ƶ�С��������ʧ���Ӷ�ģ���С���˶��켣
			Move();				// �������ӵ�λ��
			FlushBatchDraw();
		}
	}
	// ����������ͼ
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
	int mouseVX = mMouse.x - mPrevMouse.x;		// �����x����ٶ�
	int mouseVY = mMouse.y - mPrevMouse.y;		// �����y����ٶ�
	mPrevMouse.x = mMouse.x;
	mPrevMouse.y = mMouse.y;
	
	for (int i = 0; i < NumOfParticles; i++)
	{
		float dX = mParticles[i].x - mMouse.x;
		float dY = mParticles[i].y - mMouse.y;
		float d = sqrt(dX * dX + dY * dY);
		dX = d ? dX / d : 0;
		dY = d ? dY / d : 0;

		// �������������������������ڱ�ը��Χ�ڣ�ƫ����������˶�
		if (mIsLButtonDown && d < mBlowDist)
		{
			float blowAcc = (1 - (d / mBlowDist)) * 14;
			mParticles[i].vX += blowAcc * dX + 0.5f - float(rand()) / RAND_MAX;
			mParticles[i].vY += blowAcc * dY + 0.5f - float(rand()) / RAND_MAX;
		}
		// ���������������Χ�ڣ�����������˶�
		if (d < mAttractDist)
		{
			float AttractAcc = (1 - (d / mAttractDist)) * mScreenWidth * 0.0014f;
			mParticles[i].vX -= dX * AttractAcc;
			mParticles[i].vY -= dY * AttractAcc;
		}
		// ��������ڽ�����Χ�ڣ�����������ƶ��ķ����ƶ�
		if (d < mStirDist)
		{
			float StirAcc = (1 - (d / mStirDist)) * mScreenWidth * 0.00026f;
			mParticles[i].vX += mouseVX * StirAcc;
			mParticles[i].vY += mouseVY * StirAcc;
		}
		// �����ٶȳ���Ħ��ϵ��
		mParticles[i].vX *= mFriction;
		mParticles[i].vY *= mFriction;

		float avgVX = (float)fabs(mParticles[i].vX);
		float avgVY = (float)fabs(mParticles[i].vY);
		float avgV = (avgVX + avgVY) * 0.5f;
		// �����ٶ����С��0.1�����ٶȳ���һ��[0, 3]��ϵ������֤�����ٶȲ�Ϊ0
		if (avgVX < 0.1) mParticles[i].vX *= float(rand()) / RAND_MAX * 3;
		if (avgVY < 0.1) mParticles[i].vY *= float(rand()) / RAND_MAX * 3;
		// ���ӵİ뾶��������[0.9, 4.0]֮��
		float radius = avgV * 0.45f;
		radius = max(min(radius, 4.0f), 0.9f);

		float nextX = mParticles[i].x + mParticles[i].vX;
		float nextY = mParticles[i].y + mParticles[i].vY;
		// ���������������Ļ��Ե����������
		if (nextX > mScreenWidth) { nextX = (float)mScreenWidth;	mParticles[i].vX *= -1; }
		else if (nextX < 0) { nextX = 0;		mParticles[i].vX *= -1; }
		if (nextY > mScreenHeight) { nextY = (float)mScreenHeight;	mParticles[i].vY *= -1; }
		else if (nextY < 0) { nextY = 0;		mParticles[i].vY *= -1; }

		mParticles[i].x = nextX;
		mParticles[i].y = nextY;

		// ��������
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