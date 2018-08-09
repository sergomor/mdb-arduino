#ifndef moddev_CommandParser_h
#define moddev_CommandParser_h

#include "Device.h"
#include "Command.h"
#include "stdint.h"

#define CMDPARSER_BEGIN_CMD	"##"
#define CMDPARSER_PARAM_DELIM	'#'
#define CMDPARSER_END_CMD		'*'

#define MODDEV_COMMAND_BUFF_SZ	6
#define MODDEV_COMMAND_NAME_SZ	3
#define MODDEV_COMMAND_PARAM_SZ	5

namespace moddev {

class CommandParser: public Device
{
public:
	enum:state_t {
		OFF,
		WAIT_FOR_INPUT,
		ERROR_COMMAND,
		INPUT_COMMAND_NAME,
		INPUT_PARAM,
		COMMAND_EXEC,
		COMMAND_DISPATCH_ERROR
	};

	CommandParser();

	void commandAdd(void (*func)(Command&), uint16_t, uint8_t = 0);
	void commandDispatch();
	void put(char);
	void setup();

private:
	static char buffer[MODDEV_COMMAND_BUFF_SZ];
	uint16_t idx = 0;

	static const char* command_begin;
	static const char command_end;
	static const char param_delim;

	Command command;

	struct CommandDef
	{
		uint16_t id;
		void (*callback)(Command&);
		uint8_t params_count;
		CommandDef(void (*func)(Command&), uint16_t, uint8_t = 0);
	};

	LlistT<CommandDef> commands;
};
};

#endif