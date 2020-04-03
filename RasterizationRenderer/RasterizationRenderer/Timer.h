#pragma once

#include <chrono>
using namespace std::chrono;
class Timer
{
public:
	Timer() { Reset(); }
	~Timer() {}
	void Tick();				// 更新时间间隔
	void Reset();				// 计时器归零
	double DeltaTime() const;	// 两帧之间的时间间隔
	double TotalTime();			// 总时间
private:
	high_resolution_clock::time_point current;
	high_resolution_clock::time_point previous;
	high_resolution_clock::time_point start;
	double deltaTime;
	double totalTime;
};