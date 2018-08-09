
#include <SoftwareSerial.h>
#include "Device.h"
#include "CommandParser.h"
#include "Controller.h"
#include "mdb-commands.h"
#include "Link.h"

mdb::Controller c;

void setup()
{
  c.setup();
}

void loop()
{
 c.tick();
}

