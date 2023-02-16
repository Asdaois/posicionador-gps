#include "Modem.h"

Modem modem(7, 8);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  modem.begin();
  modem.connectToLTE(APNS::APN_VENEZUELA::Digitel);
}

void loop() {
  GPSData gpsData = modem.getGPSData();
  if(gpsData.status == -1) {
    Serial.println("Fallo a conseguir datos del gps");
    return;
  }

  modem.httpGetRequest(gpsData);
}


