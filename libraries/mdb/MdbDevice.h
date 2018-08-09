#ifndef mdb_Device_h
#define mdb_Device_h

#include "Device.h"
#include "MdbMaster.h"

namespace mdb {

#define MDB_DEVICE_TIME_WAIT 1000;

#define RESP_OK			(0)
#define RESP_TIMEOUT		(-2)
#define RESP_NEGATIVE	(-1)
#define RESP_NO_RETURN 	(-3)

#define POLL_STOP			true
#define POLL_CONTINUE	false

struct Response
{
	unsigned char data[36];
	unsigned int num_bytes = 0;
	unsigned char status;
};

class Device: public moddev::Device
{
private:
	static MdbMaster* _master;
	static MdbMaster* master();

protected:
	unsigned char address;
	Response response;
	uint16_t _amount;

public:

	enum { // states
		OFF,
		INIT,
		DISABLE,
		DISABLED,
		ENABLE,
		POLL,
		RESET_ATTEMPT,
		STATUS_GET,
		STATUS,
		READY,
		DEBUG
	};

	Device(unsigned char);
	~Device();

	unsigned char virtual resetCmd(){ return 0; }
	unsigned char virtual pollCmd(){ return 0; }
	unsigned char virtual setupCmd(){ return 0; }

	void ack();
	void sendCommand(unsigned char, unsigned char*, unsigned int);
	void sendCommand(unsigned char);

	int responseGet(bool = false);
	unsigned char* responseData();

	uint16_t amount(){ return _amount; }

	virtual void fillSettings(){};
   virtual void stateMachine();

   void setup();
   void tick(); 

};

};

#endif