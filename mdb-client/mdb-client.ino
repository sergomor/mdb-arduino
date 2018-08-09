#include <SoftwareSerial.h>

SoftwareSerial s(10,11); // соединяем с 11 и 10 пинами на контроллере


void setup() {

  s.begin(9600);
  Serial.begin(9600);

}

char c;

void loop() {

  if (s.available())
  {
    c = (char)s.read();
    Serial.write(c);
    if (c == '*') Serial.println("");
  }
  if (Serial.available())s.write((char)Serial.read());

}