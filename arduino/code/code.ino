#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <WiFiClientSecure.h>
#include <Adafruit_Sensor.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 15  // Pino ao qual o sensor está conectado
#define DHTTYPE DHT22  // Defina o tipo do sensor DHT22 (AM2302)

#define ReedSwitch_1 33
#define ReedSwitch_2 25
#define ReedSwitch_3 26
#define ReedSwitch_4 27

const int gasSensorPin = 27; // Pino analógico onde o sensor está conectado
const int buzzerPin = 26; // Pino digital onde o buzzer está conectado
const int threshold = 4000; // Limite de detecção de gás, ajuste conforme necessário
const int hallPin = 2; // Pino onde o sensor de efeito Hall está conectado


Adafruit_BMP280 bmp;
DHT dht(DHTPIN, DHTTYPE);


// WiFi parameters to be configured
const char* ssid = "Tiago";
const char* password = "12345678";

const char* server = "https://pap-a0bx.onrender.com";

const String DEVICE_CODE = "ESP32-WROOM-32E";


unsigned long lastTime = 0;
unsigned long timerDelay = 5000;


volatile int pulseCount = 0; // Contador de pulsos
const float calibrationFactor = 1.2; // Ajuste conforme necessário


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
  pinMode(ReedSwitch_1, INPUT);
  pinMode(ReedSwitch_2, INPUT);
  pinMode(ReedSwitch_3, INPUT);
  pinMode(ReedSwitch_4, INPUT);
  pinMode(hallPin, INPUT); // Define o pino do sensor como entrada
}

void countPulse() { pulseCount++; } // O incremento tem que ser uma callback function para o attachInterrupt

// TEMPERATURA
String readTemperature() {
  float temperature = bmp.readTemperature();
  Serial.print("Read temperature from sensor: ");
  Serial.println(temperature);
  return String(temperature);
}

// UMIDADE RELATIVA

String readHumidity() {
  float humidity = dht.readHumidity();
  Serial.print("Read humidity from sensor: ");
  Serial.println(humidity);
  return String(humidity);
}

// PRESSÃO
String readPressure() {
  float pressure = bmp.readPressure() / 100.0F; // Converte para hPa
  float seaLevelPressure = 1017.0; // Ajuste este valor conforme necessário
  pressure += (seaLevelPressure - 1013.25); // Ajusta com base na pressão ao nível do mar
  Serial.print("Read pressure from sensor: ");
  Serial.println(pressure);
  return String(pressure);
}

// ALTITUDE
String readAltitude(){
  float altitude = bmp.readAltitude(1017.9);
  Serial.print("Read altitude from sensor: ");
  Serial.println(altitude);
  return String(altitude);
}

// GÁS
String readGasLevel() {
  int gasLevel = analogRead(gasSensorPin); // Ler valor do sensor de gás
  Serial.print("Read gas level from sensor: ");
  Serial.println(gasLevel);

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
  if(gasLevel > 4000){
    return String("Mau");
  }
  return String("Bom");
}

// Wind direction
String readWindDirection(){

  int estadoReedSwitch1 = digitalRead(ReedSwitch_1);
  int estadoReedSwitch2 = digitalRead(ReedSwitch_2);
  int estadoReedSwitch3 = digitalRead(ReedSwitch_3);
  int estadoReedSwitch4 = digitalRead(ReedSwitch_4);

  if (estadoReedSwitch1 == HIGH) {
    Serial.println("Direção detetada: Norte");
    return String("Norte");
  }
  if (estadoReedSwitch2 == HIGH) {
    Serial.println("Direção detetada: Oeste");
    return String("Oeste");
  }
  if (estadoReedSwitch3 == HIGH) {
    Serial.println("Direção detetada: Sul");
    return String("Sul");
  }
  if (estadoReedSwitch4 == HIGH) {
    Serial.println("Direção detetada: Este");
    return String("Este");
  }
  return String("");
}

// Wind speed
String readWindSpeed(unsigned long lastTime){
  unsigned long currentTime = millis(); // Tempo atual
  float windSpeed = 0.0;

  detachInterrupt(digitalPinToInterrupt(hallPin)); // Desativa a interrupção para calcular a velocidade
  windSpeed = (pulseCount / (timerDelay / 1000.0)) * calibrationFactor; // Calcula a velocidade do vento em m/s
  pulseCount = 0; // Reseta o contador de pulsos
  attachInterrupt(digitalPinToInterrupt(hallPin), countPulse, FALLING); // Reativa a interrupção

  if(windSpeed >= 0){
    return String(windSpeed);
  }
  return String("");
}


void sendData(HTTPClient *http, char* type, String value) {
  if(value == "") { return; }

  String jsonPayload = "{\"type\":\"" + type + "\",\"sensor\":\"" + DEVICE_CODE  + "\",\"value\":" + value +"}";
  Serial.print("Sending " + type +": ");
  Serial.println(jsonPayload);

  int httpResponseCode = http->POST(jsonPayload);
  Serial.print("Sent " + type + "! Got Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http->getString();
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
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

            sendData(&http, "altitude", readAltitude());
            sendData(&http, "temperature", readTemperature());
            sendData(&http, "pressure", readPressure());
            sendData(&http, "humidity", readHumidity());
            sendData(&http, "air_quality", readGasLevel());
            sendData(&http, "wind_direction", readWindDirection());
            sendData(&http, "wind_speed", readWindSpeed(lastTime));

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
