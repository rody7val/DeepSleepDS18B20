// ESP8266 core
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>

// JSON encoder/decoder
#include <ArduinoJson.h>
DynamicJsonBuffer jsonBuffer;

// Depuración remota sobre telnet
#include <RemoteDebug.h>
RemoteDebug Debug;

// Dependencias para DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Init
long startMills;
float t = 0;
int vcc;
bool logged = false;
const String _id = "A";

// .env
#include "config.h"

void setup(void){
  // Inicializar el tiempo de inicio del programa en milisegundos
  startMills = millis();

  // Conectar a la red local WiFi
  WiFiManager wifi;
  if (!wifi.autoConnect(wifi_ssid, wifi_pass)) {
    Debug.println("No se puede conectar!");
  }

  // Inicializar depuración
  Debug.begin(_id);
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
  Debug.print("Tiempo -> ");
  Debug.println(millis() - startMills);

  // Obtener Voltaje del ESP8266
  vcc = ESP.getVcc();
  Debug.print("Vcc -> ");
  Debug.println(vcc);

  // Obtener Temperatura
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
  Debug.print("Temperatura -> ");
  Debug.println(t);

  // Obtener Token
  if (!logged) {
    getApiToken(api_user_email, api_user_pass);
  }

  Debug.print("Token -> ");
  Debug.println(api_user_token);
  
  // Proximo paso, enviar datos a la API
  // sendData();
}


void getApiToken(String email, String pass) {
  // Instancia cliente
  HTTPClient http;

  // Request
  http.begin("http://"+ api_host +":"+ api_port + api_login);
  http.addHeader("Content-Type", "application/json");

  // Enviar json! String con comillas escapadas:
  int httpCode = http.POST("{\"email\": \"" + email + "\",\"password\": \"" + pass + "\"}");

  // Response
  if (httpCode == 200) {

    // Obtener y decodificar JSON
    String json = http.getString();
    JsonObject& user = jsonBuffer.parseObject(json);

    // Si falla
    if (!user.success()) {
      Debug.printf("Falló la decodificación. Error nº ", http.errorToString(httpCode).c_str());
      logged = false;   // ??
      http.end();
      return;
    }

    // Token!
    api_user_token = user["token"];
    logged = true;
    Debug.println(api_user_token);
    
  } else {
    Debug.printf("Error nº ", http.errorToString(httpCode).c_str());
    logged = false;   // Server caído?
  }
  
  http.end();
}
