// ESP8266 core
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <WiFiManager.h>

// Depuración remota sobre telnet
#include "RemoteDebug.h"
RemoteDebug Debug;

// Dependencias para DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

// Configuraciones
long startMills;
const char* ID = "A";
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void){

  // Inicializar el tiempo de inicio del programa en milisegundos
  startMills = millis();

  // Conectar a la red local WiFi
  WiFiManager wifi;
  wifi.autoConnect("rod7val", "77952080");

  // Inicializar depuración
  Debug.begin(ID);
  Debug.setResetCmdEnabled(true);
  
  // Si estamos aquí, estamos conectado a la red wifi!!
  Debug.print("Conectado a WiFi! La IP es: ");
  Debug.println(WiFi.localIP());

  // Inicializar libreria DallasTemperature
  sensors.begin();

}

void loop(void){

  // Manejo de depurción
  Debug.handle();
  Debug.println(millis() - startMills);

  // Obtener y mostrar temperatura
  sensors.requestTemperatures();
  Debug.print("Temperatura -> ");
  Debug.println(sensors.getTempCByIndex(0));

}