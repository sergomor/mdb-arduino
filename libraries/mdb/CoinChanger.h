#ifndef mdb_CoinChanger_h
#define mdb_CoinChanger_h

#include "MdbDevice.h"
#include "Command.h"

namespace mdb {

class CoinChanger: public Device
{
public:
	struct settings
	{
		unsigned char feature_level;
		//unsigned int country_code;
		unsigned int scale_factor;
		unsigned char decimal_places;
		unsigned int coin_type_routing;
		unsigned char coin_type_credit[16];
	} settings;

	struct tube
	{
		unsigned int full;
		unsigned char coins[16];
		unsigned int amount;
	} tube;

	unsigned char enabled_data[5] = {0,0,0,0,0};

	enum:state_t { // states
		FEATURE_ENABLE = 20,
		COIN_WAIT,
		TUBE_STATUS_GET,
		TUBE_STATUS_UPDATE,
		TUBE_STATUS,
		COIN_ACCEPTED,
		DISPENSE,
		MANUAL_PAYOUT_REPORT,
		COIN_DISPENSED,
		PAYOUT_BUSY,
		PAYOUT,
		PAYOUT_VALUE_POLL,
		PAYOUT_VALUE_STATUS,
		PAYOUT_COMPLETE
	};

	CoinChanger();

	unsigned char virtual resetCmd(){return 0x08;}
	unsigned char virtual pollCmd(){return	0x0b;}
	unsigned char virtual setupCmd(){return 0x09;}

	void coinHandle();
	void dispenseHandle();
	void tubeStatusHandle();
	void pollSatusHandle();

	virtual void stateMachine();
	virtual void fillSettings();
	                        
	void dispense(unsigned char);
	void payout(unsigned char);
};
}; // namespace
#endif
