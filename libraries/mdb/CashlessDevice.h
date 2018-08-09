#ifndef mdb_CashlessDevice_h
#define mdb_CashlessDevice_h

#include "MdbDevice.h"

#define FIRST_CASHLESS_DEVICE_ADDR 0x60
#define SECOND_CASHLESS_DEVICE_ADDR 0x10

namespace mdb {

class CashlessDevice: public Device
{
public:

	struct SetupData
	{
		unsigned char config_data = 0;
		unsigned char feature_level = 2;
		unsigned char columns_on_display = 16;
		unsigned char rows_on_display = 2;
		unsigned char display_type = 0; // 0 - Numbers, upper case letters, blank and decimal point; 1 - Full ASCII
		operator unsigned char*(){return (unsigned char*)this;}
	} setup_data;

	struct MaxMinPrice
	{
		unsigned char config_data = 1;
		uint16_t max = 0xffff;
		uint16_t min = 0x0000;
	} max_min_price;
	
	/*
	struct MaxMinPriceExpanded
	{
		unsigned char config_data = 1;
		uint32_t max = 0xFFFFFFFF;
		uint32_t min = 0x00000000;
		uint16_t currency_code = 643; // RUB (ISO 4217)
	} max_min_price;
	*/

	struct Settings
	{
		unsigned char config_data;
		unsigned char feature_level;
		unsigned int country_code;
		unsigned char scale_factor;
		unsigned char decimal_places;
		unsigned char app_max_responce_time;
		unsigned char misc_options;
	} settings;

	struct DsplayData
	{
		unsigned char time;
		unsigned char data[32];
	} display_data;  

	enum:state_t { // states
		SETUP = 100,
		CANCEL,
		CANCELED,
		DISPLAY_REQUEST,
		SESSION_BEGIN,
		SESSION_CANCEL_REQUEST,
		VEND_APPROVED,
		VEND_DENIED,
		SESSION_END,
		SESSION_CANCELED,
		ERROR,
		BAD_COMMAND,
		APPROVED,
		REVALUE_DENIED,
		REVALUE_LIMIT_AMOUNT,
		USER_FILE_DATA,
		TIME_DATE_REQEST,
		DATA_ENTRY_REQUEST,
		DATA_ENTRY_CANCEL,
		VEND_REQUEST,
		VEND_CANCEL,
		VEND_SUCCESS,
		VEND_FAILURE,
		SESSION_COMPLETE
	};

	CashlessDevice(uint8_t num = 0);

	unsigned char virtual resetCmd(){return address;}
	unsigned char virtual pollCmd(){return	address + 2;}
	unsigned char virtual setupCmd(){return address + 1;}

	void displayHandle();
	void statusHandle();

	virtual void stateMachine();
	virtual void fillSettings();

	void vendRequest(uint16_t price);
	void vendCancel();
	void vendSuccess();
	void vendFailure();
	void sessionComplete();

};
}; // namespace
#endif /* mdb_CashlessDevice_h */
