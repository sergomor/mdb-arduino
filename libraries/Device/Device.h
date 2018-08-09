/*
  Device.h

  Copyright (c) 2016, Sergey Morgalev sergomor@gmail.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*/
#ifndef moddev_Device_h
#define moddev_Device_h

#include <Arduino.h>
#include "LlistT.h"

typedef uint8_t state_t;

namespace moddev {

//typedef uint8_t state_t;
typedef uint8_t id_t;

#define FIRST_DEVICE_ID 10

class Device
{
public:
	id_t id;

	Device();

	const uint16_t idGet() { return id; };
	virtual void setup(){}
	virtual void tick(){}

	void wait(uint32_t);
	bool waiting() { return time > millis(); };

	state_t stateGet() { return state; }
	state_t stateSet(state_t);

	virtual void onState(state_t){};
	virtual void handle(Device*){};

	void listen(Device*);
	void listen(Device*, state_t);
	void listen(Device*, state_t, state_t);
	void handleBy(void(*)(Device*));
	void handleBy(void(*)(Device*), state_t);

protected:
	struct Subscriber
	{
		enum:uint8_t { DEVICE_ALL_STATES, DEVICE_STATE, DEVICE_SWITCH_STATE, FUNCTION_ALL_STATES, FUNCTION_STATE };
		uint8_t how_to_handle;

      union {
			Device* device;
			void (*callback)(Device*);
		} ptr; 

		state_t state, to_state;

		Subscriber(Device*);
		Subscriber(Device*, state_t);
		Subscriber(Device*, state_t, state_t);
		Subscriber(void(*)(Device*));
		Subscriber(void(*)(Device*), state_t);
	};

	LlistT <Subscriber> subscribers;

private:
	static id_t counter;
	uint32_t time;

	state_t prev_state, state;

	bool isDuplicateSubscriber(Device*, uint8_t);
};
};
#endif