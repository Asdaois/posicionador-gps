 /* This is an example sketch to send battery, temperature, and GPS location data to
 *  the cloud via either HTTP GET and POST requests or via MQTT protocol. In this 
 *  sketch we will send to dweet.io, a free cloud API, as well as to ThingsBoard.io,
 *  a very powerful and free IoT platform that allows you to visualize data on dashboards.
 *  
 *  SETTINGS: You can choose to post only once or to post periodically
 *  by commenting/uncommenting line 57 ("#define samplingRate 30"). When this line is 
 *  commented out the AVR microcontroller and MCP9808 temperature sensor are put to 
 *  sleep to conserve power, but when the line is being used data will be sent to the
 *  cloud periodically. This makes it operate like a GPS tracker!
 *  
 *  PROTOCOL: You can use HTTP GET or POST requests and you can change the URL to pretty
 *  much anything you want. You can also use MQTT to publish data to different feeds
 *  on Adafruit IO. You can also subscribe to Adafruit IO feeds to command the device
 *  to do something! In order to select a protocol, simply uncomment a line in the #define
 *  section below!
 *  
 *  DWEET.IO: To check if the data was successfully sent to dweet, go to
 *  http://dweet.io/get/latest/dweet/for/{IMEI} and the IMEI number is printed at the
 *  beginning of the code but can also be found printed on the SIMCOM module itself.
 *  
 *  IoT Example Getting-Started Tutorial: https://github.com/botletics/SIM7000-LTE-Shield/wiki/GPS-Tracker-Example
 *  GPS Tracker Tutorial Part 1: https://www.instructables.com/id/Arduino-LTE-Shield-GPS-Tracking-Freeboardio/
 *  GPS Tracker Tutorial Part 2: https://www.instructables.com/id/LTE-Arduino-GPS-Tracker-IoT-Dashboard-Part-2/
 *  
 *  Author: Timothy Woo (www.botletics.com)
 *  Github: https://github.com/botletics/SIM7000-LTE-Shield
 *  Last Updated: 3/31/2021
 *  License: GNU GPL v3.0
  */

#include "BotleticsSIM7000.h" // https://github.com/botletics/Botletics-SIM7000/tree/main/src

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

#define SIMCOM_7000

// Uncomment *one* of the following protocols you want to use
// to send data to the cloud! Leave the other commented out
#define PROTOCOL_HTTP_GET         // Generic
// #define PROTOCOL_HTTP_POST        // Generic
//#define PROTOCOL_MQTT_AIO         // Adafruit IO
//#define PROTOCOL_MQTT_CLOUDMQTT   // CloudMQTT

/************************* PIN DEFINITIONS *********************************/
// For botletics SIM7000 shield
#define BOTLETICS_PWRKEY 6
#define RST 7
//#define DTR 8 // Connect with solder jumper
//#define RI 9 // Need to enable via AT commands
#define TX 2 // Microcontroller RX
#define RX 3 // Microcontroller TX
//#define T_ALERT 12 // Connect with solder jumper

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
SoftwareSerial modemSS = SoftwareSerial(TX, RX);

// Use the following line for ESP8266 instead of the line above (comment out the one above)
//SoftwareSerial modemSS = SoftwareSerial(TX, RX, false, 256); // TX, RX, inverted logic, buffer size

SoftwareSerial *modemSerial = &modemSS;

// Use this for 2G modules
#ifdef SIMCOM_2G
  Botletics_modem modem = Botletics_modem(RST);
  
// Use this one for 3G modules
#elif defined(SIMCOM_3G)
  Botletics_modem_3G modem = Botletics_modem_3G(RST);
  
// Use this one for LTE CAT-M/NB-IoT modules (like SIM7000)
// Notice how we don't include the reset pin because it's reserved for emergencies on the LTE module!
#elif defined(SIMCOM_7000) || defined(SIMCOM_7070) || defined(SIMCOM_7500) || defined(SIMCOM_7600)
Botletics_modem_LTE modem = Botletics_modem_LTE();
#endif

/****************************** OTHER STUFF ***************************************/
// For sleeping the AVR
#include <avr/sleep.h>
#include <avr/power.h>

// For temperature sensor
#include <Wire.h>

// The following line is used for applications that require repeated data posting, like GPS trackers
// Comment it out if you only want it to post once, not repeatedly every so often
#define samplingRate 10 // The time in between posts, in seconds

// The following line can be used to turn off the shield after posting data. This
// could be useful for saving energy for sparse readings but keep in mind that it
// will take longer to get a fix on location after turning back on than if it had
// already been on. Comment out to leave the shield on after it posts data.
//#define turnOffShield // Turn off shield after posting data

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
char imei[16] = {0}; // Use this for device ID
uint8_t type;
uint16_t battLevel = 0; // Battery level (percentage)
float latitude, longitude, speed_kph, heading, altitude, second;
uint16_t year;
uint8_t month, day, hour, minute;
uint8_t counter = 0;
//char PIN[5] = "1234"; // SIM card PIN

char URL[200];  // Make sure this is long enough for your request URL
char body[100]; // Make sure this is long enough for POST body
char latBuff[12], longBuff[12], locBuff[50], speedBuff[12],
     headBuff[12], altBuff[12], tempBuff[12], battBuff[12];

void setup() {
  Serial.begin(9600);
  Serial.println(F("*** SIMCom Module IoT Example ***"));

  #ifdef LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
  #endif
  
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH); // Default state

  modem.powerOn(BOTLETICS_PWRKEY); // Power on the module
  moduleSetup(); // Establishes first-time serial comm and prints IMEI


  // Unlock SIM card if needed
  // Remember to uncomment the "PIN" variable definition above
  /*
  if (!modem.unlockSIM(PIN)) {
    Serial.println(F("Failed to unlock SIM card"));
  }
  */

  // Set modem to full functionality
  modem.setFunctionality(1); // AT+CFUN=1

  // Configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
  modem.setNetworkSettings(F("internet.movistar.ve"), F("movistarve"), F("movistarve"));

  // Perform first-time GPS/GPRS setup if the shield is going to remain on,
  // otherwise these won't be enabled in loop() and it won't work!
#ifndef turnOffShield
  // Enable GPS
  while (!modem.enableGPS(true)) {
    Serial.println(F("Failed to turn on GPS, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Turned on GPS!"));

  #if !defined(SIMCOM_3G) && !defined(SIMCOM_7500) && !defined(SIMCOM_7600)
    // Disable GPRS just to make sure it was actually off so that we can turn it on
    if (!modem.enableGPRS(false)) Serial.println(F("Failed to disable GPRS!"));
    
    // Turn on GPRS
    while (!modem.enableGPRS(true)) {
      Serial.println(F("Failed to enable GPRS, retrying..."));
      delay(2000); // Retry every 2s
    }
    Serial.println(F("Enabled GPRS!"));
  #endif
#endif
}

void loop() {
  // Connect to cell network and verify connection
  // If unsuccessful, keep retrying every 2s until a connection is made
  while (!netStatus()) {
    Serial.println(F("Failed to connect to cell network, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Connected to cell network!"));

  // Measure battery level
  // Note: on the LTE shield this won't be accurate because the SIM7000
  // is supplied by a regulated 3.6V, not directly from the battery. You
  // can use the Arduino and a voltage divider to measure the battery voltage
  // and use that instead, but for now we will use the function below
  // only for testing.
  battLevel = readVcc(); // Get voltage in mV

  delay(500); // I found that this helps

  // Turn on GPS if it wasn't on already (e.g., if the module wasn't turned off)
#ifdef turnOffShield
  while (!modem.enableGPS(true)) {
    Serial.println(F("Failed to turn on GPS, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Turned on GPS!"));
#endif

  // Get a fix on location, try every 2s
  // Use the top line if you want to parse UTC time data as well, the line below it if you don't care
//  while (!modem.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)) {
  while (!modem.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude)) {
    Serial.println(F("Failed to get GPS location, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("---------------------"));
  Serial.print(F("Latitude: ")); Serial.println(latitude, 6);
  Serial.print(F("Longitude: ")); Serial.println(longitude, 6);
  Serial.print(F("Speed: ")); Serial.println(speed_kph);
  Serial.print(F("Heading: ")); Serial.println(heading);
  Serial.print(F("Altitude: ")); Serial.println(altitude);
  Serial.println(F("---------------------"));
  
  // If the shield was already on, no need to re-enable
#if defined(turnOffShield) && !defined(SIMCOM_3G) && !defined(SIMCOM_7500) && !defined(SIMCOM_7600)
  // Disable GPRS just to make sure it was actually off so that we can turn it on
  if (!modem.enableGPRS(false)) Serial.println(F("Failed to disable GPRS!"));
  
  // Turn on GPRS
  while (!modem.enableGPRS(true)) {
    Serial.println(F("Failed to enable GPRS, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Enabled GPRS!"));
#endif

  // Post something like temperature and battery level to the web API
  // Construct URL and post the data to the web API

  // Format the floating point numbers
  dtostrf(latitude, 1, 6, latBuff);
  dtostrf(longitude, 1, 6, longBuff);
  dtostrf(speed_kph, 1, 0, speedBuff);
  dtostrf(heading, 1, 0, headBuff);
  dtostrf(altitude, 1, 1, altBuff);
  dtostrf(battLevel, 1, 0, battBuff);

  // Also construct a combined, comma-separated location array
  // (many platforms require this for dashboards, like Adafruit IO):
  sprintf(locBuff, "%s,%s,%s,%s", speedBuff, latBuff, longBuff, altBuff); // This could look like "10,33.123456,-85.123456,120.5"
  
  // Construct the appropriate URL's and body, depending on request type
  // In this example we use the IMEI as device ID

  
  counter = 0; // This counts the number of failed attempts tries
  
  // You can adjust the contents of the request if you don't need certain things like speed, altitude, etc.
  sprintf(URL, "GET /update?api_key=0LM3KKFRE13A0NK9&field1=%s&field2=%s&field3=%s&field4=%s&field5=%s HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n",
          latBuff, longBuff, speedBuff, headBuff, altBuff);
          
  // Try a total of three times if the post was unsuccessful (try additional 2 times)
  while (counter < 3 && !modem.postData("api.thingspeak.com", 443, "HTTPS", URL)) { // Server, port, connection type, URL
    Serial.println(F("Failed to complete HTTP/HTTPS request..."));
    counter++; // Increment counter
    delay(1000);
  }

  sprintf(URL, "http://api.thingspeak.com/update?api_key=0LM3KKFRE13A0NK9&field1=%s&field2=%s&field3=%s&field4=%s&field5=%s", 
          latBuff, longBuff, speedBuff, headBuff, altBuff);
        
  while (counter < 3 && !modem.postData("GET", URL)) {
    Serial.println(F("Failed to post data, retrying..."));
    counter++; // Increment counter
    delay(1000);
  }
}

void moduleSetup() {
  // SIM7000 takes about 3s to turn on and SIM7500 takes about 15s
  // Press Arduino reset button if the module is still turning on and the board doesn't find it.
  // When the module is on it should communicate right after pressing reset

  // Software serial:
  modemSS.begin(115200); // Default SIM7000 shield baud rate

  Serial.println(F("Configuring to 9600 baud"));
  modemSS.println("AT+IPR=9600"); // Set baud rate
  delay(100); // Short pause to let the command run
  modemSS.begin(9600);
  if (! modem.begin(modemSS)) {
    Serial.println(F("Couldn't find modem"));
    while (1); // Don't proceed if it couldn't find the device
  }
  type = modem.type();
  Serial.println(F("Modem is OK"));
  Serial.print(F("Found "));
  switch (type) {
    case SIM800L:
      Serial.println(F("SIM800L")); break;
    case SIM800H:
      Serial.println(F("SIM800H")); break;
    case SIM808_V1:
      Serial.println(F("SIM808 (v1)")); break;
    case SIM808_V2:
      Serial.println(F("SIM808 (v2)")); break;
    case SIM5320A:
      Serial.println(F("SIM5320A (American)")); break;
    case SIM5320E:
      Serial.println(F("SIM5320E (European)")); break;
    case SIM7000:
      Serial.println(F("SIM7000")); break;
    case SIM7070:
      Serial.println(F("SIM7070")); break;
    case SIM7500:
      Serial.println(F("SIM7500")); break;
    case SIM7600:
      Serial.println(F("SIM7600")); break;
    default:
      Serial.println(F("???")); break;
  }
  
  // Print module IMEI number.
  uint8_t imeiLen = modem.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }
}

// Read the module's power supply voltage
float readVcc() {
  // Read battery voltage
  if (!modem.getBattVoltage(&battLevel)) Serial.println(F("Failed to read batt"));
  else Serial.print(F("battery = ")); Serial.print(battLevel); Serial.println(F(" mV"));

  // Read LiPo battery percentage
//  if (!modem.getBattPercent(&battLevel)) Serial.println(F("Failed to read batt"));
//  else Serial.print(F("BAT % = ")); Serial.print(battLevel); Serial.println(F("%"));

  return battLevel;
}

bool netStatus() {
  int n = modem.getNetworkStatus();
  
  Serial.print(F("Network status ")); Serial.print(n); Serial.print(F(": "));
  if (n == 0) Serial.println(F("Not registered"));
  if (n == 1) Serial.println(F("Registered (home)"));
  if (n == 2) Serial.println(F("Not registered (searching)"));
  if (n == 3) Serial.println(F("Denied"));
  if (n == 4) Serial.println(F("Unknown"));
  if (n == 5) Serial.println(F("Registered roaming"));

  if (!(n == 1 || n == 5)) return false;
  else return true;
}