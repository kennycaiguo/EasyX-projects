#pragma once

#include <ctime>
//===========================================
// Timer ��ʱ��
//===========================================
class Timer
{
public:
	Timer() { Reset(); }
	~Timer() {}
	void Tick();				// ����ʱ����
	void Reset();				// ��ʱ������
	void Update();				// ���¼�ʱ��
	double DeltaTime() const;	// ��֮֡���ʱ����
	double TotalTime() const;	// ��ʱ��
private:
	clock_t mCurrTime;
	clock_t mPrevTime;
	double mDeltaTime;
	double mTotalTime;
};

void Timer::Tick()
{
	mCurrTime = clock();
	mDeltaTime = ((double)mCurrTime - mPrevTime) / CLOCKS_PER_SEC;
	mPrevTime = mCurrTime;
}

void Timer::Reset()
{
	mCurrTime = 0;
	mPrevTime = 0;
	mDeltaTime = 0;
	mTotalTime = 0;
}

void Timer::Update()
{
	mCurrTime = clock();
	mTotalTime = (double)mCurrTime / CLOCKS_PER_SEC;
}

double Timer::DeltaTime() const
{
	return mDeltaTime;
}

double Timer::TotalTime() const
{
	return mTotalTime;
}