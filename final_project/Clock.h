
#pragma once
#include "common.h"

class Clock
{
public:
	Clock();

	void start();
	void stop();
	double getMillisecondsElapsed();

	LARGE_INTEGER start_time, end_time;
	bool finished;
};
