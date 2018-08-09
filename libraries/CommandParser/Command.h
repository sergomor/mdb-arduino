#ifndef moddev_rpc_Command_H
#define moddev_rpc_Command_H

#include "LlistT.h"
#include "stdint.h"

namespace moddev {

class Command
{
public:
	struct Param
	{
		int16_t val;
		Param():val(0){}
		Param(int16_t i): val(i){}
		operator int16_t(){ return val; }
	};

	uint16_t id;
	LlistT<Param> params;

	Command():id(0){};
	Command(int16_t _id, uint16_t = 0): id(_id){}
	void reuse(uint16_t _id, uint16_t = 0){ id = _id; params.reuse(); }
};
};

#endif