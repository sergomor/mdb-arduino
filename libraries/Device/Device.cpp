#include "Device.h"

namespace moddev {

Device::Subscriber::Subscriber(Device* d): how_to_handle(DEVICE_ALL_STATES)
{
	ptr.device = d;
}

Device::Subscriber::Subscriber(Device* d, state_t on): state(on), how_to_handle(DEVICE_STATE)
{
	ptr.device = d;
}

Device::Subscriber::Subscriber(Device* d, state_t on, state_t to): state(on), to_state(to), how_to_handle(DEVICE_SWITCH_STATE)
{
	ptr.device = d;
}

Device::Subscriber::Subscriber(void(*callback)(Device*)): how_to_handle(FUNCTION_ALL_STATES)
{
	ptr.callback = callback;
}

Device::Subscriber::Subscriber(void(*callback)(Device*), state_t on): state(on), how_to_handle(FUNCTION_STATE)
{
	ptr.callback = callback;
}

id_t Device::counter = FIRST_DEVICE_ID;

Device::Device(): time(0)
{
	id = Device::counter++;
}

state_t Device::stateSet(state_t _state)
{
	prev_state = state;
	state = _state;
	onState(state);
	Subscriber* s;
	for (auto i = subscribers.begin(); i != subscribers.end(); i = subscribers.next())
	{
		s = &(*i);
		switch (s->how_to_handle)
		{
			case Subscriber::DEVICE_ALL_STATES:
				s->ptr.device->handle(this);
				break;
		  	case Subscriber::DEVICE_STATE:
		  		if (state == s->state) s->ptr.device->handle(this);
		  		break;
		  	case Subscriber::DEVICE_SWITCH_STATE:
		  		if (state == s->state) s->ptr.device->stateSet(s->to_state);
		  		break;
		  	case Subscriber::FUNCTION_ALL_STATES:
		  		s->ptr.callback(this);
		  		break;
		  	case Subscriber::FUNCTION_STATE:
		  		if (state == s->state) s->ptr.callback(this);
		  		break;				
		}
	}
	return prev_state;
}

bool Device::isDuplicateSubscriber(Device* device, uint8_t how_to_handle)
{
	Subscriber* s;
	bool duplicated = false;
	for (auto i = subscribers.begin(); i != subscribers.end(); i = subscribers.next())
	{
		s = &(*i);
		if (s->ptr.device == device && s->how_to_handle == how_to_handle)
		{
			duplicated = true;
			break;
		}
	}
	return duplicated;
}

void Device::listen(Device* device)
{
	if (!device->isDuplicateSubscriber(this, Subscriber::DEVICE_ALL_STATES)) device->subscribers.add(Subscriber(this));
}

void Device::listen(Device* device, state_t on_state)
{
	if (!device->isDuplicateSubscriber(this, Subscriber::DEVICE_ALL_STATES)) device->subscribers.add(Subscriber(this, on_state));
}

void Device::listen(Device* device, state_t on_state, state_t to_state)
{
	device->subscribers.add(Subscriber(this, on_state, to_state));
}

void Device::handleBy(void(*callback)(Device*))
{
	subscribers.add(Subscriber(callback));
}

void Device::handleBy(void(*callback)(Device*), state_t on_state)
{
	subscribers.add(Subscriber(callback, on_state));
}

void Device::wait(uint32_t mls)
{
	time = mls + millis();
}

};