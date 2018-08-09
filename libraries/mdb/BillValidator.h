#ifndef mdb_BillValidator_h
#define mdb_BillValidator_h

#include "MdbDevice.h"

namespace mdb {

class BillValidator: public Device
{
private:

public:
	struct Settings
	{
		unsigned char feature_level;
		//unsigned int country_code;
		unsigned int scale_factor;
		unsigned char decimal_places;
		unsigned int stacker_capacity;
		unsigned int security_levels;
		bool escrow_enabled;
		unsigned char bill_types[16];
	};

	Settings settings;

	unsigned char enabled_data[4] = {0,0,0,0};

	enum:state_t { // states
		BILL_WAIT = 10,
		ESCROW_POSITION,
		ESCROW,
		ESCROW_WAIT,
		BILL_RETURNED,
		STACK, // для отправки купюры в стопку
		STACKED // состояние что купюра в стопке
	};

	BillValidator();

	unsigned char virtual resetCmd(){return 0x30;}
	unsigned char virtual pollCmd(){return	0x33;}
	unsigned char virtual setupCmd(){return 0x31;}

	void billHandle();
	void statusHandle();

	virtual void stateMachine();
	virtual void fillSettings();

	int SetSecurity(unsigned int);

	//virtual String ToString();
};
}; // namespace
#endif /* MdbBillValidator_h */
