#include "Timer.h"

void Timer::Tick()
{
	current = high_resolution_clock::now();
	milliseconds duration = duration_cast<milliseconds>(current - previous);
	deltaTime = (double)duration.count();
	previous = current;
}

void Timer::Reset()
{
	start = high_resolution_clock::now();
	deltaTime = 0;
	totalTime = 0;
}

double Timer::DeltaTime() const
{
	return deltaTime;
}

double Timer::TotalTime()
{
	milliseconds duration = duration_cast<milliseconds>(current - start);
	totalTime = (double)duration.count();
	return totalTime;
}
