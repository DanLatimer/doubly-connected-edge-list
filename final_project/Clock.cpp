
#include "stdafx.h"
#include "Clock.h"

Clock::Clock() : finished(false) 
{
	// start timer
	QueryPerformanceCounter(&start_time);
}

double Clock::getMillisecondsElapsed()
{
	// stop timer
	QueryPerformanceCounter(&end_time);

	// get ticks per second
	LARGE_INTEGER frequency;        // ticks per second
	QueryPerformanceFrequency(&frequency);

	// compute and print the elapsed time in millisec
	const double elapsedTime = (end_time.QuadPart - start_time.QuadPart) * 1000.0 / frequency.QuadPart;
	
	return elapsedTime;
}
