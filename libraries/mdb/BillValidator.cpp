#include "BillValidator.h"

namespace mdb {

BillValidator::BillValidator(): Device(0x30)
{
}

void BillValidator::stateMachine()
{
	unsigned char data = 0;
	char prev_state;

	Device::stateMachine();

	switch (stateGet())
	{
		case READY:
		case ENABLE:
			memset(enabled_data, 255, 4);
			sendCommand(0x34, enabled_data, 4);
			stateSet(POLL);
			break;
		case DISABLE:
			stateSet(DISABLED);
			memset(enabled_data, 0, 4);
			sendCommand(0x34, enabled_data, 4);
			break;
	  	case ESCROW:
			data = 0;
			sendCommand(0x35, &data, 1);
			stateSet(ESCROW_WAIT);
			break;
		case STACK:
			data = 1;
			sendCommand(0x35, &data, 1);
			stateSet(ESCROW_WAIT);
			break;	
		case POLL:
		case ESCROW_POSITION:
		case ESCROW_WAIT:
		case STACKED:
			sendCommand(pollCmd());
			if (response.num_bytes > 1)
			{
				ack();
				if (response.data[0] & 128) // 10000000
				{
					//Bills Accepted
					billHandle();
					stateSet(POLL);
				}
			}
			break;
	}		
}

void BillValidator::billHandle()
{
	_amount = settings.scale_factor * settings.bill_types[response.data[0] & 15] / pow(10, settings.decimal_places);
	
	byte rout = response.data[0] >> 4 & 7; // x1110000
	switch (rout)
	{
		case 0: // 000: BILL STACKED
			stateSet(STACKED);
			break;
		case 1: // 001: ESCROW POSITION
			stateSet(ESCROW_POSITION);
			break;
		case 2: // 010: BILL RETURNED
			stateSet(BILL_RETURNED);
			break;
		case 3: // 011: BILL TO RECYCLER
			break;
		case 4: // 100: DISABLED BILL REJECTED
			break;
		case 5: // 101: BILL TO RECYCLER � MANUAL FILL
			break;
		case 6: // 110: MANUAL DISPENSE
			break;
		case 7: // 111: TRANSFERRED FROM RECYCLER TO CASHBOX
			break;
	}
}

void BillValidator::fillSettings()
{
	settings.feature_level = response.data[0];
	//settings.country_code = (((unsigned int) response.data[1]) << 8) | ((unsigned int) response.data[2]);
	settings.scale_factor = (((unsigned int) response.data[3]) << 8) | ((unsigned int) response.data[4]);
	settings.decimal_places = response.data[5];
	settings.stacker_capacity = (((unsigned int) response.data[6]) << 8) | ((unsigned int) response.data[7]);
	settings.security_levels = (((unsigned int) response.data[8]) << 8) | ((unsigned int) response.data[9]);
	settings.escrow_enabled = response.data[10] == 0xFF;
   // Get the value of each bill type.
	for (int i = 0; i < 16; i++) settings.bill_types[i] = response.data[11 + i];
}

int BillValidator::SetSecurity(unsigned int securitySettings)
{
	unsigned char securityBits[2];
	securityBits[0] = securitySettings && 0xFF;
	securityBits[1] = (securitySettings >> 8) && 0xFF;
	sendCommand(0x32, securityBits, 2);
	return 0;//responseGet();
}

}; //namespace