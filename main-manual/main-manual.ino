#include <SoftwareSerial.h>

SoftwareSerial modemSerial(7, 8); // RX, TX

void setup() {
  modemSerial.begin(9600);
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Initializing modem...");
  
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
  sendATCommand("AT", 1000); // Verifica si el modem está disponible y listo para recibir comandos
  sendATCommand("AT+CFUN=1", 1000); // Habilita la funcionalidad total del modem
  sendATCommand("AT+COPS?", 1000); // Verifica la operadora de red
  Serial.println("Modem initialized.");
  
  // Configura la conexión a la red LTE
  sendATCommand("AT+CGATT=1", 1000); // Conecta el modem a la red
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"LTE\"", 1000); // Establece el tipo de conexión a LTE
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"internet.comcel.com.co\"", 3000); // Establece el nombre de punto de acceso (APN)
  sendATCommand("AT+SAPBR=1,1", 3000); // Inicia la sesión de conexión a Internet
  sendATCommand("AT+SAPBR=2,1", 3000); // Verifica el estado de la conexión
  
  // Verifica si se ha conectado exitosamente a la red LTE
  if (modemSerial.find("OK")) {
    Serial.println("Connected to LTE network.");
  } else {
    Serial.println("Error connecting to LTE network.");
  }
}

void loop() {

}

void sendATCommand(String command, int delayTime) {
  modemSerial.println(command);
  delay(delayTime);
  while (modemSerial.available()) {
    Serial.write(modemSerial.read());
  }
}

