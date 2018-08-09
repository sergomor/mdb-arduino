#ifndef moddev_Link_h
#define moddev_Link_h

#include <Stream.h>
#include "CommandParser.h"
#include "Device.h"
#include <SoftwareSerial.h>

namespace moddev {

class Link: public Device
{
public:
	Link(CommandParser*, SoftwareSerial*);
	Link();

	void parserSet(CommandParser*);
	void streamSet(SoftwareSerial*);

	void send(Command&);

	virtual void tick();

private:
	SoftwareSerial* stream;
	CommandParser* parser;
};

}; //namespace
#endif