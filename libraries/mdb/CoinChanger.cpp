#include "CoinChanger.h"

namespace mdb {


CoinChanger::CoinChanger(): Device(0x08)
{
}

void CoinChanger::stateMachine()
{                                                                                          
	char prev_state;
	unsigned char data[2];

	Device::stateMachine();

	switch (stateGet())
	{
		case READY:
			// enable ack for tube status
			stateSet(FEATURE_ENABLE);
			break;
		case FEATURE_ENABLE:
			memset(enabled_data, 255, 5);
			enabled_data[0] = 0x01;
			sendCommand(0x0f, enabled_data, 5);
			stateSet(TUBE_STATUS_GET);
			break;
		case TUBE_STATUS_GET:
			sendCommand(0x0a);
			if (response.num_bytes > 17)
			{
				// tube status received
				ack();
				tubeStatusHandle();
				stateSet(TUBE_STATUS);
			}
			break;

		case TUBE_STATUS:
			// enable to accept coins
			stateSet(ENABLE);
			break;
		case ENABLE:
			memset(enabled_data, 255, 4);
			sendCommand(0x0c, enabled_data, 4);
			stateSet(POLL);
			break;
		case DISABLE:
			stateSet(DISABLED);
			memset(enabled_data, 0, 4);
			sendCommand(0x0c, enabled_data, 4);
			break;
		case DISPENSE:
			data[0] = _amount | 255;
			sendCommand(0x0d, data, 1);
			stateSet(POLL);
			break;
		case POLL:
			sendCommand(pollCmd());
			if (response.num_bytes > 1)
			{
				ack();
				if (response.data[0] & 64)
				{
					coinHandle();
					stateSet(POLL);
				}
				// status
				if (2 == response.data[0])
				{
					// payout busy
					stateSet(PAYOUT_BUSY);
				}
			}
			break;
		case PAYOUT_BUSY:
			sendCommand(pollCmd());
			if (0 == response.data[0])
			{
				_amount = settings.coin_type_credit[_amount & 15];
				// trugger dispensed event
				//stateSet(COIN_DISPENSED);
				//stateSet(ENABLE);
				stateSet(MANUAL_PAYOUT_REPORT);
			}
			break;
		case MANUAL_PAYOUT_REPORT:
			data[0] = 0x07;
			sendCommand(0x0f, data, 1);
			for (int i=0; i < 16; i++)
			{
				if (response.data[i])
				{
					_amount = settings.coin_type_credit[i];
					break;
				}
			}
			stateSet(COIN_DISPENSED);
			stateSet(POLL);
			break;
		case PAYOUT:
			data[0] = 0x02;
			data[1] = _amount;
			sendCommand(0x0f, data, 2);
			stateSet(PAYOUT_VALUE_POLL);
			break;
		
		case PAYOUT_VALUE_POLL:
			data[0] = 0x04;
			sendCommand(0x0f, data, 1);
			if (response.num_bytes == 1 && response.data[0] == 0)
			{
				stateSet(PAYOUT_VALUE_STATUS);
			}
			else
			{
				ack();
			}
			break;
		
		case PAYOUT_VALUE_STATUS:
			data[0] = 0x03;
			sendCommand(0x0f, data, 1);
			if (response.num_bytes > 1)
			{
				ack();
				
				_amount = 0;
				for (int i=0; i < 16; i++)
				{
					_amount += response.data[i] * settings.coin_type_credit[i];
				}
				
				stateSet(PAYOUT_COMPLETE);
				stateSet(POLL);
			}
			else
			{
				stateSet(PAYOUT_VALUE_STATUS);
				//amount = response.data[0];
				//stateSet(PAYOUT_COMPLETE);
				//stateSet(ENABLED);
			}
			break;
	}		
}

void CoinChanger::fillSettings()
{
	settings.feature_level = response.data[0];
	//settings.country_code = (((unsigned int) response.data[1]) << 8) | ((unsigned int) response.data[2]);
	settings.scale_factor = (((unsigned int) response.data[3]) << 8) | ((unsigned int) response.data[4]);
	settings.decimal_places = response.data[5];
	settings.coin_type_routing = (((unsigned int) response.data[6]) << 8) | ((unsigned int) response.data[7]);
	for (int i = 0; i < 16; i++) settings.coin_type_credit[i] = response.data[7 + i];
}

void CoinChanger::coinHandle()
{
	_amount = settings.coin_type_credit[response.data[0] & 15];	
	byte rout = response.data[0] >> 4 & 3;			// x1110000
	switch (rout)
	{
		case 0: // 00: CASH BOX
			stateSet(COIN_ACCEPTED);
			break;
		case 1: // 01: TUBES
			stateSet(COIN_ACCEPTED);
			break;
		case 2: // 01: NOT USED
			break;
		case 3: // 01: REJEC
			break;
	}
}

void CoinChanger::dispenseHandle()
{
	unsigned char num_coins = response.data[0] >> 4 & 7;
	_amount = settings.coin_type_credit[response.data[0] & 15];
	stateSet(COIN_DISPENSED);
	//stateSet(ENABLED);
}

void CoinChanger::tubeStatusHandle()
{
	tube.full = (((unsigned int) response.data[0]) << 8) | ((unsigned int) response.data[1]);
	tube.amount = 0;
	for (int i = 0; i < 16; i++)
	{
		tube.coins[i] = response.data[2 + i];
		tube.amount += settings.coin_type_credit[i] * tube.coins[i];
	}
}

void CoinChanger::pollSatusHandle()
{
	unsigned char status = response.data[0];
	switch (status)
	{
		case 1: // Escrow request
			break;
		case 2: // Changer Payout Busy
			stateSet(PAYOUT_BUSY);
			break;
		case 3: // No Credit
			break;
		case 4: // Defective Tube Sensor
			break;
		case 5: // Double Arrival
			break;
		case 6: // Acceptor Unplugged
			break;
		case 7: // tube Jam
			break;
		case 8: // ROM checksum error
			break;
		case 9: // Coin Routing Error
			break;
		case 10: // Changer Busy
			break;
		case 11: // Changer was Reset
			break;
		case 12: // Coin Jam
			break;
		case 13: // Possible Credited Coin Remova
			break;
	}
}

void CoinChanger::dispense(unsigned char amount)
{
	_amount = 0;
	for (int i=0; i < 6; i++)
	{
		if (amount == settings.coin_type_credit[i])
		{
			_amount = i;
			break;
		}
	}
	// one coin
	_amount |= 16;
	stateSet(DISPENSE);
}

void CoinChanger::payout(unsigned char amount)
{
	_amount = amount;
	stateSet(PAYOUT);
}

}; //namespace