#pragma once

#include <ctime>
//===========================================
// Timer 计时器
//===========================================
class Timer
{
public:
	Timer() { Reset(); }
	~Timer() {}
	void Tick();				// 更新时间间隔
	void Reset();				// 计时器归零
	void Update();				// 更新计时器
	double DeltaTime() const;	// 两帧之间的时间间隔
	double TotalTime() const;	// 总时间
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