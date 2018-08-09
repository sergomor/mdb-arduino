#include "Controller.h"
#include "Command.h"
#include "mdb-commands.h"
#include "link-data.h"

namespace mdb {

using namespace moddev;

Controller::Controller(): moddev::Device()
{
	serial = new SoftwareSerial(LINK_CONTROLLER_RX_PIN, LINK_CONTROLLER_TX_PIN);
	serial->begin(LINK_BAUD_RATE);

	link.streamSet(serial);
	link.parserSet(&parser);

	serial->println("ll");
}

Controller::~Controller()
{
	delete serial;
}

void Controller::setup()
{
	ControllerCommands::init(this);
	
	listen(&validator);
	listen(&changer);
	listen(&cashless);

	link.setup();
	validator.setup();
	changer.setup();
	cashless.setup();

	wait(time_to_poll);
}

void Controller::tick()
{
	link.tick();
	switch(poll_dev % 3)
	{
		case 0: validator.tick(); break;
		case 1: changer.tick(); break;
		case 2: cashless.tick(); break;
	}
	if (waiting()) return;
	wait(time_to_poll);
	poll_dev++;
}

void Controller::coinChangerHandle()
{
	uint16_t amount = changer.amount();
	switch (changer.stateGet())
	{
		case CoinChanger::READY:
			break;
		case CoinChanger::STATUS:
			changerSettingsGet();
			break;
		case CoinChanger::COIN_ACCEPTED:
			sendToLink(MDB_ANS_CHANGER_COIN_ACCEPTED, &amount);
			break;
		case CoinChanger::COIN_DISPENSED:
			sendToLink(MDB_ANS_CHANGER_COIN_DISPENSED, &amount);
			break;
		case CoinChanger::TUBE_STATUS:
			changerTubeGet();
			break;
		case CoinChanger::PAYOUT_COMPLETE:
			sendToLink(MDB_ANS_CHANGER_PAYOUT, &amount);
			break;
		case CoinChanger::DEBUG:
			sendToLink(MDB_ANS_DEBUG_CHANGER, &amount);
			break;
	}
}

void Controller::validatorHandle()
{
	uint16_t amount = validator.amount();
	switch(validator.stateGet())
	{
		case BillValidator::STACKED:
			sendToLink(MDB_ANS_VALIDATOR_BILL_STACKED, &amount);
			break;
		case BillValidator::READY:
			break;
		case BillValidator::ESCROW_POSITION:
			sendToLink(MDB_ANS_VALIDATOR_BILL_IN_ESCROW, &amount);
			break;
		case BillValidator::BILL_RETURNED:
			sendToLink(MDB_ANS_VALIDATOR_BILL_ESCROW, &amount);
			break;
		case BillValidator::STATUS:
			validatorSettingsGet();
			break;
		case BillValidator::DEBUG:
			sendToLink(MDB_ANS_DEBUG_VALIDATOR, &amount);
			break;
	}
}

void Controller::cashlessHandle()
{
	uint16_t amount = cashless.amount();
	switch(cashless.stateGet())
	{
		case CashlessDevice::STATUS:
			cashlessSettingsGet();
			break;
	}
}

void Controller::handle(Device* d)
{
	if (d->id == validator.id) validatorHandle();
	if (d->id == changer.id) coinChangerHandle();
	if (d->id == cashless.id) cashlessHandle();
}

void Controller::sendToLink(uint16_t cmd, uint8_t length, uint16_t* buff)
{
	command.reuse(cmd);
	for (uint8_t i = 0; i < length; i++) command.params.add(buff[i]);
	link.send(command);
}

void Controller::changerSettingsGet()
{
	command.reuse(MDB_ANS_CHANGER_STATUS);
	command.params.add(changer.settings.feature_level);
	command.params.add((uint16_t)changer.settings.scale_factor);
	command.params.add((uint16_t)changer.settings.decimal_places);
	command.params.add((uint16_t)changer.settings.coin_type_routing);
	link.send(command);
}

void Controller::changerTubeGet()
{
	command.reuse(MDB_ANS_CHANGER_TUBE_STATUS);
	command.params.add((int)changer.tube.full);
	command.params.add((int)changer.tube.amount);
	for (uint8_t i=0; i < 5; i++) command.params.add(changer.tube.coins[i]);
	link.send(command);
}

void Controller::changerCoinTypesGet()
{
	command.reuse(MDB_ANS_CHANGER_COIN_TYPES);
	for (uint8_t i=0; i < 6; i++) command.params.add(changer.settings.coin_type_credit[i]);
	link.send(command);
}

void Controller::validatorSettingsGet()
{
	command.reuse(MDB_ANS_VALIDATOR_STATUS);
	command.params.add(validator.settings.feature_level);
	command.params.add((uint16_t)validator.settings.scale_factor);
	command.params.add((uint16_t)validator.settings.decimal_places);
	command.params.add((uint16_t)validator.settings.stacker_capacity);
	command.params.add((uint16_t)validator.settings.security_levels);
	command.params.add((uint16_t)validator.settings.escrow_enabled);
	link.send(command);
}

void Controller::validatorBillTypesGet()
{
	command.reuse(MDB_ANS_VALIDATOR_BILL_TYPES);
	for (uint8_t i=0; i < 6; i++) command.params.add(validator.settings.bill_types[i]);
	link.send(command);
}

void Controller::cashlessSettingsGet()
{
	command.reuse(MDB_ANS_CASHLESS_STATUS);
	command.params.add(cashless.settings.config_data);
	command.params.add(cashless.settings.feature_level);
	command.params.add(cashless.settings.scale_factor);
	command.params.add(cashless.settings.decimal_places);
	command.params.add(cashless.settings.app_max_responce_time);
	command.params.add(cashless.settings.misc_options);
	link.send(command);
}

}; // namespace
