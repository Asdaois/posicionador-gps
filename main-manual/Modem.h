#ifndef MODEM_H
#define MODEM_H

#include <SoftwareSerial.h>
#include "Apn.h"

class Modem {
  public:
    Modem(int rx, int tx) : modemSerial(rx, tx) {
        Serial.println("Initializing modem...");      
    };
    void begin();
    void connectToLTE(APNS::APN_VENEZUELA unAPN);
    void sendATCommand(String command, int delayTime = 1000);
    
  private:
    SoftwareSerial modemSerial;
};

void Modem::begin() {
  modemSerial.begin(9600);

  // Verificar calidad de señal
  sendATCommand("AT+CSQ", 1000);
  if (modemSerial.find("+CSQ: 99, 99")) {
    Serial.println("Error: No signal detected.");
    return;
  }
  
  // Verificar estado de tarjeta SIM
  sendATCommand("AT+CPIN?", 1000);
  if (modemSerial.find("ERROR")) {
    Serial.println("Error: SIM card not inserted or not activated.");
    return;
  }
  // Inicializa el modem
  sendATCommand("AT"); // Verifica si el modem está disponible y listo para recibir comandos
  sendATCommand("AT+CFUN=1"); // Habilita la funcionalidad total del modem
  sendATCommand("AT+COPS?"); // Verifica la operadora de red
  Serial.println("Modem initialized.");
}

void Modem::connectToLTE(APNS::APN_VENEZUELA unAPN) {
  ApnDatos apn = CrearApn(unAPN);
   // Configura la conexión a la red LTE
  sendATCommand("AT+CGATT=1"); // Conecta el modem a la red
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"LTE\""); // Establece el tipo de conexión a LTE
  sendATCommand("AT+SAPBR=3,1,\"APN\"," + String("\"") + apn.apn + "\"", 3000); // Establece el nombre de punto de acceso (APN)
  sendATCommand("AT+SAPBR=1,1", 3000); // Inicia la sesión de conexión a Internet
  sendATCommand("AT+SAPBR=2,1", 3000); // Verifica el estado de la conexión

   // Verifica si se ha conectado exitosamente a la red LTE
  if (modemSerial.find("OK")) {
    Serial.println("Connected to LTE network.");
  } else {
    Serial.println("Error connecting to LTE network.");
  }
}

void Modem::sendATCommand(String command, int delayTime = 1000) {
  modemSerial.println(command);
  delay(delayTime);
  while (modemSerial.available()) {
    Serial.write(modemSerial.read());
  }
}

#endif