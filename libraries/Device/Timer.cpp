#include <Arduino.h>
#include "Timer.h"

namespace moddev {

void Timer::tick()
{
	if (OFF == stateGet() || waiting()) return;
	
	uint32_t ms = millis();

	if (time <= ms)
	{
		stop();
	}
	else
	{
		uint32_t seconds = (time - ms) / 1000;
		if (seconds != count)
		{
			count = seconds;
			stateSet(TICK);
			wait(200);
		}
	}
}

void Timer::start(uint32_t seconds)
{
	time = 1000 * seconds + millis();
	count = seconds;
	stateSet(START);
}

void Timer::stop()
{
	stateSet(STOP);
}

void Timer::off()
{
	stateSet(OFF);
}

uint32_t Timer::seconds()
{
	return count + 1;
}

};