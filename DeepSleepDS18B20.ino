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

// Dependencias para Sensoress DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define SensorBus 2
OneWire SensorModel(SensorBus);
DallasTemperature SensorsController(&SensorModel);

// .env
typedef void (*GeneralMessageFunction) ();
#include "config.h"

// Callback Guardar Temperatura
void GuardarTemperatura() {
  // Cazar Objeto JSON
  JsonObject& sensor = jsonBuffer.parseObject(response);
  sensor_saved_id = sensor["sensor"]["_id"];

  // Si falla
  if (!sensor.success()) {
    Debug.printf("Falló la decodificación");
    sensor.printTo(Debug);
    return;
  }

  // Registro del Sensor Guardado!
  Debug.print("ID de Registro -> ");
  Debug.println(sensor_saved_id);
}

// Callback Obtener Token
void GetApiToken() {
  // Cazar Objeto JSON
  JsonObject& user = jsonBuffer.parseObject(response);
  api_user_token = user["token"];

  // Si falla
  if (!user.success()) {
    Debug.printf("Fallo la decodificacion");
    user.printTo(Debug);
    return;
  }
  
  // Token!
  Debug.print("Token -> ");
  Debug.println(api_user_token);
  _logged = true;
}

// Prototype
void promesa(String url, String path, String json, GeneralMessageFunction next);

// Servicio http
void promesa(String url, String path, String json, GeneralMessageFunction next) {
  // Instancia cliente
  HTTPClient http;

  // Request
  http.begin(url + path);
  http.addHeader("Authorization", String("token ") + api_user_token);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Cache-Control", "no-cache");

  // Enviar json! String con comillas escapadas:
  int httpCode = http.POST(json);

  // Response
  if (httpCode == 200) {

    // Manejo de next() para ejecutar el <i>callback</i> (la siguiente función a ejecutar).
    response = http.getString();
    http.end();
    next();
    return;

  } else if (httpCode == 403) {

    Debug.println("Token expirado!");
    _logged = false;
    http.end();
    return;

  } else {
    Debug.println("Error server ->");
    Debug.println(http.getString());
  }

  http.end();
}


void setup(void){
  // Inicializar el tiempo de inicio del programa en milisegundos
  _startMills = millis();

  // Conectar a la red local WiFi
  WiFiManager wifi;
  if (!wifi.autoConnect(wifi_ssid, wifi_pass)) {
    Debug.println("No se puede conectar!");
  }

  // Inicializar depuración
  Debug.begin(_placa);
  Debug.setResetCmdEnabled(true);
  
  // Si estamos aquí, estamos conectado a la red wifi!!
  Debug.print("Conectado a WiFi! La IP es: ");
  Debug.println(WiFi.localIP());
  
  // Inicializar Sensores
  SensorsController.begin();

  // Setear resolución entre 9 y 12 bits, menos es más rapido
  SensorsController.setResolution(sr1, 10);
  SensorsController.setResolution(sr2, 10);
}

void loop(void){
  // Manejo de depurción
  Debug.handle();

  // Obtener Voltaje del ESP8266
  _vcc = ESP.getVcc();
  Debug.print("Vcc -> ");
  Debug.println(_vcc);

  // Obtener y mostrar Temperaturas
  SensorsController.requestTemperatures();
  Debug.print("Temperaturas -> ");
  temp[0] = SensorsController.getTempC(sr1);
  temp[1] = SensorsController.getTempC(sr2);
  Debug.println("[" + String(temp[0]) + ", " + String(temp[1]) + "]");

  String url;

  // Middleware controler
  if (!_logged) {

    // Objeto JSON_User
    static String json_user = String("{") +
      "\"email\"   :\""+ api_user_email +"\","+
      "\"password\":\""+ api_user_pass +"\"" +
    "}";

    // Obtener Token
    url = "http://"+ api_host +":"+ api_port;
    promesa(url, api_login, json_user, GetApiToken);
    return;
  }

  // Datos JSON_Sensor
  static String json_sensor = String("{") +
    "\"temp\":"+ "[" + String(temp[0]) +","+ String(temp[1]) +"]"+","+
    "\"prefix\":"+ "[\""+ sr1_prefix +"\",\""+ sr2_prefix +"\"]"+","+    
    "\"time\":"+ String(_startMills) +","+
    "\"vcc\":"+ String(_vcc) +","+
    "\"placa\":\""+ _placa +"\","+
    "\"sector\":\""+ _sector +"\","+
    "\"pin\":\""+ _pin +"\""+
  "}";

  // Guardar Temperatura
  url = "http://"+ api_host +":"+ api_port;
  promesa(url, api_new_sensor, json_sensor, GuardarTemperatura);
}

