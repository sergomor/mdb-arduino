#include "CommandParser.h"

namespace moddev {

char CommandParser::buffer[MODDEV_COMMAND_BUFF_SZ];
const char* CommandParser::command_begin = CMDPARSER_BEGIN_CMD;
const char CommandParser::command_end = CMDPARSER_END_CMD;
const char CommandParser::param_delim = CMDPARSER_PARAM_DELIM;

CommandParser::CommandDef::CommandDef(void (*func)(Command&), uint16_t _id, uint8_t _params_count):
	callback(func), id(_id), params_count(_params_count)	
{
}

CommandParser::CommandParser(): Device()
{
	stateSet(WAIT_FOR_INPUT);
}

void CommandParser::commandAdd(void (*func)(Command&), uint16_t id, uint8_t params_count)
{
	commands.add(CommandDef(func, id, params_count));
}

void CommandParser::commandDispatch()
{
	bool found = false;
	if (!commands.itemCount())
	{
		stateSet(COMMAND_DISPATCH_ERROR);
	}
	else
	{
		CommandDef* cmd;
		for (auto i = commands.begin(); i != commands.end(); i = commands.next())
		{
			cmd = &(*i);
			if (command.id == cmd->id && cmd->params_count == command.params.itemCount() && cmd->callback)
			{
				cmd->callback(command);
				found = true;
				break;
			}
		}
		if (!found) stateSet(COMMAND_DISPATCH_ERROR);
	}
	command.id = 0;
	command.params.reuse();
}

void CommandParser::setup()
{
	stateSet(OFF);
}

void CommandParser::put(char c)
{
	state_t state = stateGet();
	if (c == command_end)
	{
		// run command if no params
		if (state == INPUT_COMMAND_NAME)
		{
			command.id = atoi(buffer);
		}
		// run command if params ended
		if (state == INPUT_PARAM)
		{
			int val = atoi(buffer);
			command.params.add(val);
		}
		idx = 0;
		stateSet(COMMAND_EXEC);
		commandDispatch();
		stateSet(WAIT_FOR_INPUT);
		return;
	}

	switch (state)
	{
		case ERROR_COMMAND:
			idx = 0;
			stateSet(WAIT_FOR_INPUT);
			break;
		case WAIT_FOR_INPUT:
			buffer[idx++] = c;
			buffer[idx] = NULL;
			if (strlen(buffer) > strlen(command_begin)) stateSet(ERROR_COMMAND);
			else if (strlen(buffer) == strlen(command_begin) && 0 == strcmp(buffer, command_begin))
			{
				idx = 0;
				stateSet(INPUT_COMMAND_NAME);
			}
			break;
		case INPUT_COMMAND_NAME:
			if (c == param_delim)
			{
				command.id = atoi(buffer);
				idx = 0;
				stateSet(INPUT_PARAM);
			}
			else buffer[idx++] = c;
			buffer[idx] = NULL;
			if (strlen(buffer) > MODDEV_COMMAND_NAME_SZ) stateSet(ERROR_COMMAND);
			break;
		case INPUT_PARAM:
			if (c == param_delim)
			{
				int val = atoi(buffer);
				command.params.add(val);
				idx = 0;
				stateSet(INPUT_PARAM);
			}
			else buffer[idx++] = c;
			buffer[idx] = NULL;
			if (strlen(buffer) > MODDEV_COMMAND_PARAM_SZ) stateSet(ERROR_COMMAND);
			break;
	}
}

};
