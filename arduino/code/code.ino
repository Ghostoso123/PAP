#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <WiFiClientSecure.h>
#include <Adafruit_Sensor.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 15  // Pino ao qual o sensor está conectado
#define DHTTYPE DHT22  // Defina o tipo do sensor DHT22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

const int gasSensorPin = 27; // Pino analógico onde o sensor está conectado
const int buzzerPin = 26; // Pino digital onde o buzzer está conectado
const int threshold = 4000; // Limite de detecção de gás, ajuste conforme necessário

Adafruit_BMP280 bmp;

// WiFi parameters to be configured
const char* ssid = "Tiago";
const char* password = "12345678";

const char* server = "https://pap-a0bx.onrender.com";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

const String DEVICE_CODE = "ESP32-WROOM-32E";

void setup(void) { 
  Serial.begin(9600);
  Serial.println("Just Starting...");

  if (!bmp.begin(0x76)) { // Endereço I2C padrão do BMP280
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, /* Modo operacional. */ 
                  Adafruit_BMP280::SAMPLING_X2, /* Temp. sobreamostragem */ 
                  Adafruit_BMP280::SAMPLING_X16, /* Sobreamostragem de pressão */ 
                  Adafruit_BMP280::FILTER_X16, /* Filtrando. */ 
                  Adafruit_BMP280::STANDBY_MS_500); /* Tempo de espera. */
  
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  pinMode(gasSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

// TEMPERATURA
void sendTemperature(HTTPClient *http) {
  float temperature = bmp.readTemperature();
  Serial.print("Read temperature from sensor: ");
  Serial.println(temperature);

  String temperatureJson = "{\"type\":\"temperature\",\"sensor\":\"" + DEVICE_CODE +  "\",\"value\":" + String(temperature) +"}";
  Serial.print("Sending temperature: ");
  Serial.println(temperatureJson);

  int httpTemperatureResponseCode = http->POST(temperatureJson);
  Serial.print("Sent Temperature! Got Response code: ");
  Serial.println(httpTemperatureResponseCode);

  if (httpTemperatureResponseCode > 0) { // Check if the request was successful
    String response = http->getString(); // Get the response payload
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpTemperatureResponseCode);
  }
}

// UMIDADE RELATIVA

void sendHumidity(HTTPClient *http) {
  float humidity = dht.readHumidity();
  Serial.print("Read humidity from sensor: ");
  Serial.println(humidity);

  String humidityJson = "{\"type\":\"humidity\",\"sensor\":\"" + DEVICE_CODE +  "\",\"value\":" + String(humidity) +"}";
  Serial.print("Sending humidity: ");
  Serial.println(humidityJson);

  int httpHumidityResponseCode = http->POST(humidityJson);
  Serial.print("Sent Humidity! Got Response code: ");
  Serial.println(httpHumidityResponseCode);

  if (httpHumidityResponseCode > 0) { // Check if the request was successful
    String response = http->getString(); // Get the response payload
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpHumidityResponseCode);
  }
}

// PRESSÃO
void sendPressure(HTTPClient *http) {
  float pressure = bmp.readPressure() / 100.0F; // Converte para hPa
  float seaLevelPressure = 1017.0; // Ajuste este valor conforme necessário
  pressure += (seaLevelPressure - 1013.25); // Ajusta com base na pressão ao nível do mar
  Serial.print("Read pressure from sensor: ");
  Serial.println(pressure);

  String pressureJson = "{\"type\":\"pressure\",\"sensor\":\"" + DEVICE_CODE +  "\",\"value\":" + String(pressure) +"}";
  Serial.print("Sending pressure: ");
  Serial.println(pressureJson);

  int httpPressureResponseCode = http->POST(pressureJson);
  Serial.print("Sent Pressure! Got Response code: ");
  Serial.println(httpPressureResponseCode);

  if (httpPressureResponseCode > 0) { // Check if the request was successful
    String response = http->getString(); // Get the response payload
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpPressureResponseCode);
  }
}

// ALTITUDE
void sendAltitude(HTTPClient *http){
  float altitude = bmp.readAltitude(1017.9);
  Serial.print("Read altitude from sensor: ");
  Serial.println(altitude);

  String altitudeJson = "{\"type\":\"altitude\",\"sensor\":\"" + DEVICE_CODE  + "\",\"value\":" + String(altitude) +"}";
  Serial.print("Sending altitude: ");
  Serial.println(altitudeJson);

  int httpAltitudeResponseCode = http->POST(altitudeJson);
  Serial.print("Sent Altitude! Got Response code: ");
  Serial.println(httpAltitudeResponseCode);

  if (httpAltitudeResponseCode > 0) { // Check if the request was successful
    String response = http->getString(); // Get the response payload
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpAltitudeResponseCode);
  }
}

// GÁS
void sendGasLevel(HTTPClient *http) {
  int gasLevel = analogRead(gasSensorPin); // Ler valor do sensor de gás
  Serial.print("Read gas level from sensor: ");
  Serial.println(gasLevel);

  String gasJson = "{\"type\":\"gas\",\"sensor\":\"" + DEVICE_CODE +  "\",\"value\":" + String(gasLevel) +"}";
  Serial.print("Sending gas level: ");
  Serial.println(gasJson);

  int httpGasResponseCode = http->POST(gasJson);
  Serial.print("Sent Gas Level! Got Response code: ");
  Serial.println(httpGasResponseCode);

  if (httpGasResponseCode > 0) { // Check if the request was successful
    String response = http->getString(); // Get the response payload
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpGasResponseCode);
  }

  if (gasLevel > threshold) { // Se o nível de gás ultrapassar o limite
    // Emitir som alternado no buzzer
    tone(buzzerPin, 1000); // Emitir som de 1000 Hz
    delay(500); // Esperar 500 ms
    noTone(buzzerPin); // Parar o som
    delay(500); // Esperar 500 ms
    tone(buzzerPin, 1500); // Emitir som de 1500 Hz
    delay(500); // Esperar 500 ms
    noTone(buzzerPin); // Parar o som
    delay(500); // Esperar 500 ms
  } else {
    noTone(buzzerPin); // Desligar o buzzer se o nível de gás estiver abaixo do limite
  }
}

void loop(void) {  
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); // This is only for testing purposes, use proper SSL certificates in production

        HTTPClient http;
        if (http.begin(client, server)) {
            http.addHeader("Content-Type", "application/json");

            sendTemperature(&http);
            sendAltitude(&http);
            sendPressure(&http);
            sendHumidity(&http);
            sendGasLevel(&http);
            sendGasLevel(&http, "altitude", bmp.readAltidude);

            http.end();
        } else {
            Serial.println("Failed to connect to server");
        }
    } else {
        Serial.println("Disconnected");
    }
    lastTime = millis();
  }
}
