#include "Modem.h"

Modem modem(7, 8);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  modem.begin();
  modem.connectToLTE(APNS::APN_VENEZUELA::Movistar);
}

void loop() {

}


