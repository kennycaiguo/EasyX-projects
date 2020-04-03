#pragma once

#include <chrono>
using namespace std::chrono;
class Timer
{
public:
	Timer() { Reset(); }
	~Timer() {}
	void Tick();				// ����ʱ����
	void Reset();				// ��ʱ������
	double DeltaTime() const;	// ��֮֡���ʱ����
	double TotalTime();			// ��ʱ��
private:
	high_resolution_clock::time_point current;
	high_resolution_clock::time_point previous;
	high_resolution_clock::time_point start;
	double deltaTime;
	double totalTime;
};