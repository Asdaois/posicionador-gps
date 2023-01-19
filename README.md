# Posicionamiento GPS con arduino uno y modulo SIM7000

[Datasheet del modulo gps](https://www.waveshare.com/wiki/SIM7000E_NB-IoT_HAT)

## Api para IOT con gps

```cpp
const char server[]   = "api.thingspeak.com";
const char resource[] = "/update??api_key=0LM3KKFRE13A0NK9&field1=";
```

## Conexion a la red

Apn Movistar
```cpp 
const char apn[] = "internet.movistar.ve";
const char gprsUser[] = "movistarve";
const char gprsPass[] = "movistarve";
```