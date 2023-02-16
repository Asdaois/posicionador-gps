#define TINY_GSM_MODEM_SIM7000

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1

// or Software Serial on Uno, Nano
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 9600

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// set GSM PIN, if any
#define GSM_PIN ""

// Set phone numbers, if you want to test SMS and Calls
// #define SMS_TARGET  "+380xxxxxxxxx"
// #define CALL_TARGET "+380xxxxxxxxx"

// Your GPRS credentials, if any
const char apn[] = "internet.digitel.ve";


// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourSSID";
const char wifiPass[] = "YourWiFiPass";

// Server details to test TCP/SSL
const char server[]   = "api.thingspeak.com";

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif

String crearRecurso(String latitud, String longitud) {
  DBG("lat:" + latitud + ", lon:" + longitud);
  return "/update??api_key=DB2F7LOEOFENB7G3&field1=" + latitud + "&field2=" + longitud;
}

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  DBG("Wait...");
  delay(6000);

  // Set GSM module baud rate
  TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  // SerialAT.begin(9600);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  modem.restart();

  String name = modem.getModemName();
  DBG("Modem Name:", name);

  SerialAT.write("AT+CMEE=1");
  SerialAT.write("AT+CSQ");
  SerialAT.write("AT+CREG?");
  SerialAT.write("AT+CGATT=1");
  SerialAT.write("AT+CPIN?");
  SerialAT.write("AT+CREG?");
  SerialAT.write("AT+CEER");
  SerialAT.write("AT+CSQ");
}

void loop() {
  DBG("Enabling GPS/GNSS/GLONASS and waiting 1s for warm-up");
  modem.enableGPS();
  
  float lat      = 0;
  float lon      = 0;
  float speed    = 0;
  float alt      = 0;
  int   vsat     = 0;
  int   usat     = 0;
  float accuracy = 0;
  int   year     = 0;
  int   month    = 0;
  int   day      = 0;
  int   hour     = 0;
  int   min      = 0;
  int   sec      = 0;
  for (int8_t i = 15; i; i--) {
    DBG("Requesting current GPS/GNSS/GLONASS location");
    if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                     &year, &month, &day, &hour, &min, &sec)) {
      DBG("Latitude:", String(lat, 8), "\tLongitude:", String(lon, 8));
      DBG("Speed:", speed, "\tAltitude:", alt);
      DBG("Visible Satellites:", vsat, "\tUsed Satellites:", usat);
      DBG("Accuracy:", accuracy);
      DBG("Year:", year, "\tMonth:", month, "\tDay:", day);
      DBG("Hour:", hour, "\tMinute:", min, "\tSecond:", sec);
      String gps_raw = modem.getGPSraw();
      DBG("GPS/GNSS Based Location String:", gps_raw);
      break;
    } else {
      DBG("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
      delay(15000L);
    }
  }

  DBG("Connecting to", apn);
  if (!modem.gprsConnect(apn)) {
    delay(10000);
    DBG("No se pudo conectar a la red movistar");
  }

  bool res = modem.isGprsConnected();
  DBG("GPRS status:", res ? "connected" : "not connected");

  String ccid = modem.getSimCCID();
  DBG("CCID:", ccid);

  String imei = modem.getIMEI();
  DBG("IMEI:", imei);

  String imsi = modem.getIMSI();
  DBG("IMSI:", imsi);

  String cop = modem.getOperator();
  DBG("Operator:", cop);

  IPAddress local = modem.localIP();
  DBG("Local IP:", local);

  int csq = modem.getSignalQuality();
  DBG("Signal quality:", csq);

  TinyGsmClient client(modem, 0);
  const int     port = 80;
  DBG("Connecting to", server);
  if (!client.connect(server, port)) {
    DBG("... failed");
  } else {
    // Make a HTTP GET request:
    client.print(String("GET ") + crearRecurso(String(lat), String(lon)) + " HTTP/1.0\r\n");
    client.print(String("Host: ") + server + "\r\n");
    client.print("Connection: close\r\n\r\n");

    // Wait for data to arrive
    uint32_t start = millis();
    while (client.connected() && !client.available() &&
           millis() - start < 15000L) {
      delay(100);
    };
  }
}