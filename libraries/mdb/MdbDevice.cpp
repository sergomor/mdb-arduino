#include "MdbDevice.h"

namespace mdb {

MdbMaster* Device::_master = NULL;

Device::Device(unsigned char addr): address(addr), moddev::Device()
{
}

Device::~Device()
{
	if (_master) delete _master;
}

MdbMaster* Device::master()
{
	if (!_master) _master = new MdbMaster();
	return _master;
}

void Device::ack()
{
	master()->SendAck();
}

void Device::sendCommand(unsigned char command, unsigned char *dataBytes, unsigned int dataByteCount)
{
	master()->SendCommand(address, command, dataBytes, dataByteCount);
	responseGet();
}

void Device::sendCommand(unsigned char command)
{
	unsigned char dataBytes[1];
	sendCommand(command, NULL, 0);
}

int Device::responseGet(bool poll)
{
	response.num_bytes = 0;	
	int res = master()->GetResponse(response.data, &(response.num_bytes));
	if (res == -1)								response.status = RESP_TIMEOUT;
	else if (response.data[0] == NAK)	response.status = RESP_NEGATIVE;
	else if (response.num_bytes == 0)	response.status = RESP_NO_RETURN;
	else											response.status = RESP_OK;
	return response.status;
}

unsigned char* Device::responseData()
{
	return response.data;
}

void Device::stateMachine()
{
	switch (stateGet())
	{
		case INIT:
			// send reset command
			stateSet(RESET_ATTEMPT);
			sendCommand(resetCmd());
			break;

		case RESET_ATTEMPT:
			// wait for response JUST RESET
			sendCommand(pollCmd());
			if (response.num_bytes > 1)
			{
				ack();
				// if device is ready
				// get device settings
				stateSet(STATUS_GET);
		 	}
			break;

		case STATUS_GET:
			// settings
			sendCommand(setupCmd());
			if (response.num_bytes > 20)
			{
				ack();
				fillSettings();
				stateSet(STATUS);
				stateSet(READY);
			}
			break;
	}
}

void Device::setup()
{
	stateSet(OFF);
}

void Device::tick()
{
	if (waiting()) return;
	stateMachine();
	wait(50);
}
};
