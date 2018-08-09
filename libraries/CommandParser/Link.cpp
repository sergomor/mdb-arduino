#include "Link.h"

namespace moddev {

Link::Link(CommandParser* _parser, SoftwareSerial* _stream): parser(_parser), stream(_stream), Device()
{
}

Link::Link(): Link(NULL, NULL)
{
}

void Link::streamSet(SoftwareSerial* _stream)
{
	stream = _stream;
}

void Link::parserSet(CommandParser* _parser)
{
	parser = _parser;
}

void Link::send(Command& c)
{
	stream->flush();
	stream->print(CMDPARSER_BEGIN_CMD);
	stream->print(c.id);
	
	for (auto i = c.params.begin(); i != c.params.end(); i = c.params.next())
	{
		stream->print(CMDPARSER_PARAM_DELIM);
		stream->print(*i);
	}
	
	stream->print(CMDPARSER_END_CMD);	
}

void Link::tick()
{
	if (NULL != parser && stream->available()) parser->put(stream->read());
}

};