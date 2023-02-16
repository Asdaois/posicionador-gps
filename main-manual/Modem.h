#include "HardwareSerial.h"
#ifndef MODEM_H
#define MODEM_H

#include <SoftwareSerial.h>
#include "Apn.h"

struct GPSData {
  float latitude;
  float longitude;
  int altitude;
  int status = 1;
};

class Modem {
public:
  Modem(int rx, int tx)
    : modemSerial(rx, tx) {
    Serial.println("Initializing modem...");
  };
  void begin();
  void connectToLTE(APNS::APN_VENEZUELA unAPN);
  void sendATCommand(String command, int delayTime = 1000);
  void httpGetRequest(GPSData gpsData);
  GPSData getGPSData();

private:
  SoftwareSerial modemSerial;
  String lastResponse;
};

void Modem::begin() {
  modemSerial.begin(9600);
  Serial.println("Reseteando mode para evitar complicaciones");
  sendATCommand("AT+CFUN=1,1", 10000);

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
  sendATCommand("AT");         // Verifica si el modem está disponible y listo para recibir comandos
  sendATCommand("AT+CFUN=1");  // Habilita la funcionalidad total del modem
  sendATCommand("AT+CBAND=ALL_MODE"); // Seleccionar todas las bandas para intentar funcionar en modo global
  sendATCommand("AT+COPS?");   // Verifica la operadora de red
  Serial.println("Modem initialized.");
}

void Modem::connectToLTE(APNS::APN_VENEZUELA unAPN) {
  ApnDatos apn = CrearApn(unAPN);
  // Configura la conexión a la red LTE
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"LTE\"");                             // Establece el tipo de conexión a LTE
  sendATCommand("AT+SAPBR=3,1,\"APN\"," + String("\"") + apn.apn + "\"", 3000);  // Establece el nombre de punto de acceso (APN)
  sendATCommand("AT+CGATT=1");                                                   // Conecta el modem a la red
  sendATCommand("AT+SAPBR=1,1", 3000);                                           // Inicia la sesión de conexión a Internet
  sendATCommand("AT+SAPBR=2,1", 3000);                                           // Verifica el estado de la conexión
  sendATCommand("AT+COPS?");
  sendATCommand("AT+CGNAPN");
  sendATCommand("AT+CNACT=1");
  sendATCommand("AT+CNACT?");
  // Verifica si se ha conectado exitosamente a la red LTE
  if (modemSerial.find("OK")) {
    Serial.println("Connected to LTE network.");
  } else {
    Serial.println("Error connecting to LTE network.");
  }
}

void Modem::sendATCommand(String command, int delayTime = 1000) {
  Serial.println(command);
  modemSerial.println(command);
  delay(delayTime);
  String response = "";
  while (modemSerial.available()) {
    int raw = modemSerial.read();
    Serial.println("--->" + raw);
    response += String(raw);
  }
  this->lastResponse = response;
}

GPSData Modem::getGPSData() {
  sendATCommand("AT+CGNSPWR=1", 1000);  // Encender el GPS
  sendATCommand("AT+CGPSINF=0", 1000);  // Solicita la información GPS
  Serial.println(this->lastResponse);
  String response = this->lastResponse;  // Lee la respuesta del modem

  int index = response.indexOf("+CGPSINF: 0,");
  if (index != -1) {
    GPSData data;
    int commaIndex = response.indexOf(',', index + 12);
    data.status = response.substring(index + 12, commaIndex).toInt();
    int commaIndex2 = response.indexOf(',', commaIndex + 1);
    data.latitude = response.substring(commaIndex + 1, commaIndex2).toFloat();
    commaIndex = response.indexOf(',', commaIndex2 + 1);
    data.longitude = response.substring(commaIndex2 + 1, commaIndex).toFloat();
    commaIndex2 = response.indexOf(',', commaIndex + 1);
    data.altitude = response.substring(commaIndex + 1, commaIndex2).toInt();
    return data;  // Devuelve los datos GPS
  }

  GPSData errorData;
  errorData.status = -1;
  return errorData;  // En caso de error
}

void Modem::httpGetRequest(GPSData gpsData) {
  // Configurar parametros HTTP
  sendATCommand("AT+SHCONF=\"URL\",\"http://api.thingspeak.com\"");
  sendATCommand("AT+SHCONF=\"BODYLEN\",1024");
  sendATCommand("AT+SHCONF=\"HEADERLEN\",350");
  sendATCommand("AT+SHCONN"); // Realizar Conexion

  // Verificarconexion a servidor
  if (modemSerial.find("+SHSTATE: 1")) {
    sendATCommand("AT+SHCHEAD"); // Limpiar conexion
    // Modificar cabeza para enviar datos
    sendATCommand("AT+SHAHEAD=\"User-Agent\",\"curl/7.47.0\""); 
    sendATCommand("AT+SHAHEAD=\"Cache-control\",\"no-cache\"");
    sendATCommand("AT+SHAHEAD=\"Connection\",\"keep-alive\"");
    sendATCommand("AT+SHAHEAD=\"Accept\",\"*/*\"");
    String request = "AT+SHREQ=\"/update?api_key=0LM3KKFRE13A0NK9&field1=" + String(gpsData.latitude) + "&field2=" + String(gpsData.longitude) + "&field3=" + String(gpsData.altitude) + "&status=" + String(gpsData.status) + "\",1";
    sendATCommand(request);

    if (modemSerial.find("+SHREQ: \"GET\",200")) {
      Serial.println("Data successfully sent to API.");
    } else {
      Serial.println("Error sending data to API.");
    }
  } else {
    Serial.println("Error connecting to API.");
  }
}

#endif