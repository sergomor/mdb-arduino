#include "Device.h"
#include "CoinChanger.h"

mdb::CoinChanger c;

void coinCallback(moddev::Device* d)
{
  int state =  d->stateGet();
  switch(state)
  {
    case mdb::CoinChanger::COIN_ACCEPTED:
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      break;
   case mdb::Device::STATUS:
      if (c.settings.feature_level != 3)
      {
          delay(50);
          c.stateSet(mdb::CoinChanger::INIT); 
       }
       break;
  }

   //if (0 == ++trigger%3) c.payout(10);
   
}

void setup()
{
  pinMode(13, OUTPUT);
  // устанавливаем обработчик
  c.handleBy(coinCallback);
  // запускаем монетоприемник
  c.stateSet(mdb::CoinChanger::INIT);  
}

void loop()
{
  // слушаем монетоприемник
  c.tick();
}
