#ifndef mdb_Controller_h
#define mdb_Controller_h

#include "BillValidator.h"
#include "CoinChanger.h"
#include "CommandParser.h"
#include "CashlessDevice.h"
#include "Link.h"
#include "ControllerCommands.h"
#include <SoftwareSerial.h>

namespace mdb {

class Controller: public moddev::Device
{
public:
	friend ControllerCommands;

	SoftwareSerial* serial;

   moddev::Link link;
	
	Controller();
	~Controller();

	void sendToLink(uint16_t, uint8_t = 0, uint16_t* = nullptr);

	void validatorHandle();
	void coinChangerHandle();
	void cashlessHandle();

	// external response
	// bill validator
	void validatorSettingsGet();
	void validatorBillTypesGet();

	// coin changer
	void changerCoinTypesGet();
	void changerSettingsGet();
	void changerTubeGet();

	// cashless device
	void cashlessSettingsGet();

	void handle(Device*);
	void setup();
	void tick();	

private:
	//SoftwareSerial* serial;
	moddev::Command command;
	moddev::CommandParser parser;

	BillValidator validator;
	CoinChanger changer;
	CashlessDevice cashless;

	uint32_t time_to_poll = 1000;
	uint8_t poll_dev = 0;
};
}; // namespace

#endif