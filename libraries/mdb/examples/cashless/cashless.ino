#include "Device.h"
#include "CashlessDevice.h"

mdb::CashlessDevice c;

void cdCallback(moddev::Device* d)
{
  int state =  d->stateGet();
  switch(state)
  {
    case mdb::CashlessDevice::VEND_SUCCESS:
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      break;
  }
}

void setup()
{
  pinMode(13, OUTPUT);
  c.handleBy(cdCallback);
  c.stateSet(mdb::CashlessDevice::INIT);  
}

void loop()
{
  c.tick();
}
