#include "ControllerCommands.h"
#include "CommandParser.h"
#include "mdb-commands.h"
#include "Controller.h"
#include "utils.h"


#define PARSE_CMD controller->parser.commandAdd([](moddev::Command& c)
#define PARSE_CMD1 controller->parser.commandAdd([](moddev::Command& c)

namespace mdb {

Controller* ControllerCommands::controller = nullptr;

void ControllerCommands::init(Controller* c)
{
	controller = c;
	PARSE_CMD {
		controller->validator.stateSet(BillValidator::STACK);
	}, MDB_CMD_STACK);
	
	PARSE_CMD {
		uint16_t type = *(c.params.begin());
		switch (type)
		{
			case 1: controller->validator.stateSet(BillValidator::INIT); break;
			case 2: controller->changer.stateSet(CoinChanger::INIT);; break;
			case 3: controller->cashless.stateSet(CashlessDevice::INIT); break;
		}
	}, MDB_CMD_RESET);

	PARSE_CMD {
		controller->validator.stateSet(BillValidator::STATUS_GET);
	}, MDB_CMD_VALIDATOR_STATUS);

	PARSE_CMD {
		controller->changer.stateSet(CoinChanger::STATUS_GET);
	}, MDB_CMD_CHANGER_STATUS);

	PARSE_CMD {
		controller->validatorBillTypesGet();
	}, MDB_CMD_BILL_TYPES);

	PARSE_CMD {
		controller->changerCoinTypesGet();
	}, MDB_CMD_COIN_TYPES);

	PARSE_CMD {
		uint16_t type = *(c.params.begin());
		type == 1 ? controller->validator.stateSet(BillValidator::ENABLE) : controller->changer.stateSet(CoinChanger::ENABLE);
	}, MDB_CMD_ENABLE);

	PARSE_CMD {
		uint16_t type = *(c.params.begin());
		type == 1 ? controller->validator.stateSet(BillValidator::DISABLE) : controller->changer.stateSet(CoinChanger::DISABLE);
	}, MDB_CMD_DISABLE);

	PARSE_CMD {
		controller->validator.stateSet(BillValidator::ESCROW);
	}, MDB_CMD_ESCROW);

	PARSE_CMD {
		uint8_t coin = *(c.params.begin());
		controller->changer.dispense(coin);
	}, MDB_CMD_DISPENSE);

	PARSE_CMD {
		controller->changer.stateSet(CoinChanger::TUBE_STATUS_GET);
	}, MDB_CMD_TUBE_STATUS);

	PARSE_CMD {
		uint8_t coin = *(c.params.begin());
		controller->changer.payout(coin);
	}, MDB_CMD_PAYOUT);

	PARSE_CMD {
		uint16_t ram = ::freeRam();
		controller->sendToLink(MDB_ANS_FREERAM, &ram);
	}, MDB_FREERAM);
}

};