#ifndef moddev_Timer_h
#define moddev_Timer_h

#include "Device.h"
#include "stdint.h"

namespace moddev {

class Timer: public Device
{
public:
	enum:state_t {
		// states
		OFF, START, TICK, STOP
	};

	Timer(): time(0), count(0), Device(){}

	virtual void tick();

	void start(uint32_t);
	void off();
	void stop();
	uint32_t seconds();

private:
	uint32_t time;
	uint32_t count;
};
};
#endif