# Posicionamiento GPS con arduino uno y modulo SIM7000

[Datasheet del modulo gps](https://www.waveshare.com/wiki/SIM7000E_NB-IoT_HAT)

## Api para IOT con gps

```cpp
const char server[]   = "api.thingspeak.com";
const char resource[] = "/update??api_key=0LM3KKFRE13A0NK9&field1=";
```

## Conexion a la red

```
apn = internet.movistar.ve;
user and password = movistarve;
```
```Apn digitel
internet.digitel.ve
gprs.digitel.ve
gprsweb.digitel.ve
AT+CSTT="gprsweb.digitel.ve"
```