#ifndef mdb_ControllerCommands_h
#define mdb_ControllerCommands_h

#include "Command.h"

namespace mdb {

class Controller;

class ControllerCommands
{
private:
	static Controller* controller;

public:
	void static init (Controller*);
};
};
#endif