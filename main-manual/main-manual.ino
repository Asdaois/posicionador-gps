#include "Modem.h"

Modem modem(7, 8);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Initializing modem...");
  

  Serial.begin(9600);
  modem.begin();
  modem.connectToLTE();
}

void loop() {

}


