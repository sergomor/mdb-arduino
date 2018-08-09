#include "CashlessDevice.h"

namespace mdb {

CashlessDevice::CashlessDevice(uint8_t num): Device(num ? FIRST_CASHLESS_DEVICE_ADDR : SECOND_CASHLESS_DEVICE_ADDR)
{
}

void CashlessDevice::stateMachine()
{
	unsigned char data = 0;
	char prev_state;

	struct
	{
		uint8_t req = 0;
		uint16_t price;
		uint16_t item_number = 0xffff;
		operator unsigned char*(){return (unsigned char*)this;}
	} vend_data;
	vend_data.price = _amount;

	struct
	{
		uint8_t req = 2;
		uint16_t item_number = 0xffff;
		operator unsigned char*(){return (unsigned char*)this;}
	} success_data;

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
				stateSet(SETUP);
		 	}
			break;

		case SETUP:
			// Config Data
			sendCommand(address + 1, setup_data, sizeof(setup_data));
			if (response.data[0] == 1)
			{
				ack();
				fillSettings();
				stateSet(STATUS);
				stateSet(READY);
			}
			break;

		case READY:
		case ENABLE:
			data = 1;
			sendCommand(address + 4, &data, 1);
			stateSet(POLL);
			break;

		case DISABLE:
			data = 0;
			sendCommand(address + 4, &data, 1);
			stateSet(DISABLED);
			break;

		case CANCEL:
			data = 2;
			sendCommand(address + 4, &data, 1);
			stateSet(CANCELED);
			break;

		case POLL:
			sendCommand(pollCmd());
			if (response.num_bytes > 1)
			{
				ack();
				switch (response.data[0])
				{
					case 1: // got settings
						fillSettings();
						stateSet(POLL);
						break;
					case 2: // got display request
						displayHandle();
						stateSet(DISPLAY_REQUEST);
						stateSet(POLL);
						break;
					case 3:
						stateSet(SESSION_BEGIN);
						break;
					case 4:
						stateSet(SESSION_CANCEL_REQUEST);
						break;
					case 5:
						_amount = response.data[1];
						stateSet(VEND_APPROVED);
						break;
					case 6:
						stateSet(VEND_DENIED);
						break;
					case 7:
						stateSet(SESSION_END);
						break;
					case 8:
						stateSet(SESSION_CANCELED);
						break;
					case 10:
						stateSet(ERROR);
						break;
					case 11:
						stateSet(BAD_COMMAND);
						break;
					case 13:
						stateSet(APPROVED);
						break;
					case 14:
						stateSet(REVALUE_DENIED);
						break;
					case 15:
						stateSet(REVALUE_LIMIT_AMOUNT);
						break;
					case 16:
						stateSet(USER_FILE_DATA);
						break;
					case 17:
						stateSet(TIME_DATE_REQEST);
						break;
					case 18:
						stateSet(DATA_ENTRY_REQUEST);
						break;
					case 19:
						stateSet(DATA_ENTRY_CANCEL);
						break;
				}
			}
			break;

			case VEND_REQUEST:
				sendCommand(address + 3, vend_data, sizeof(vend_data));
				break;

			case VEND_CANCEL:
				data = 1;
				sendCommand(address + 3, &data, 1);
				break;

			case VEND_SUCCESS:
				sendCommand(address + 3, success_data, sizeof(success_data));
				break;

			case VEND_FAILURE:
				data = 3;
				sendCommand(address + 3, &data, 1);
				break;

			case SESSION_COMPLETE:
			  	data = 4;
				sendCommand(address + 3, &data, 1);
				break;
	}		
}

void CashlessDevice::vendRequest(uint16_t amount)
{
	_amount = amount;
	stateSet(VEND_REQUEST);
}

void CashlessDevice::vendCancel()
{
	stateSet(VEND_CANCEL);
}

void CashlessDevice::vendSuccess()
{
	stateSet(VEND_SUCCESS);
}

void CashlessDevice::vendFailure()
{
	stateSet(VEND_FAILURE);
}

void CashlessDevice::sessionComplete()
{
	stateSet(SESSION_COMPLETE);
}

void CashlessDevice::fillSettings()
{
	settings.config_data = response.data[0];
	settings.feature_level = response.data[1];
	settings.country_code = (((unsigned int) response.data[2]) << 8) | ((unsigned int) response.data[3]);
	settings.scale_factor = response.data[4];
	settings.decimal_places = response.data[5];
	settings.app_max_responce_time = response.data[6];
	settings.misc_options = response.data[7];
}

void CashlessDevice::displayHandle()
{
	display_data.time = response.data[1];
	for (int i = 0; i < 32; i++) display_data.data[i] = response.data[2 + i];
}

}; //namespace