#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <WiFiClient.h>
#include <Adafruit_Sensor.h>

#include <HTTPClient.h>
#include <WiFi.h>





Adafruit_BMP280 bmp;

// WiFi parameters to be configured
const char* ssid = "Tiago";
const char* password = "12345678";
const char* serverName = "http://pap-a0bx.onrender.com/";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

const String DEVICE_CODE = "ESP32-WROOM-32E";

void setup(void)
{ 
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


}

/**
 *  TEMPERATURE
 */
void sendTemperature(HTTPClient *http){
  float temperature = bmp.readTemperature();
  Serial.print("Read temperature from sensor: ");
  Serial.println(temperature);

  String temperatureJson = "{\"type\":\"temperature\",\"sensor\":\"" + DEVICE_CODE +  "\",\"value\":" + String(temperature) +"}";
  Serial.print("Sending temperature: ");
  Serial.println(temperatureJson);

  int httpTemperatureResponseCode = http->POST(temperatureJson);
  Serial.print("Sent Temperature! Got Response code: ");
  Serial.println(httpTemperatureResponseCode);
}


/**
 *  PRESSURE
 */
void sendPressure(HTTPClient *http){
  float pressure = bmp.readPressure();
  Serial.print("Read pressure from sensor: ");
  Serial.println(pressure);

  String pressureJson = "{\"type\":\"pressure\",\"sensor\":\"" + DEVICE_CODE +  "\",\"value\":" + String(pressure) +"}";
  Serial.print("Sending pressure: ");
  Serial.println(pressureJson);

  int httpPressureResponseCode = http->POST(pressureJson);
  Serial.print("Sent Pressure! Got Response code: ");
  Serial.println(httpPressureResponseCode);
}

/**
 *  ALTITUDE
 */
void sendAltitude(HTTPClient *http){
  float altitude = bmp.readAltitude(1013.25);
  Serial.print("Read altitude from sensor: ");
  Serial.println(altitude);

  String altitudeJson = "{\"type\":\"altitude\",\"sensor\":\"" + DEVICE_CODE  + "\",\"value\":" + String(altitude) +"}";
  Serial.print("Sending altitude: ");
  Serial.println(altitudeJson);

  int httpAltitudeResponseCode = http->POST(altitudeJson);
  Serial.print("Sent Altitude! Got Response code: ");
  Serial.println(httpAltitudeResponseCode);
}

/**
 * WindSpeed
 */
void sendWindSpeed(HTTPClient *http){
  float windSpeed = 10; //substiruir este 10 pela logica  de ler o sensor
  
  Serial.print("Read wind speed from sensor: ");
  Serial.println(windSpeed);


  String windSpeedJson = "{\"type\":\"wind-speed\",\"sensor\":\"" + DEVICE_CODE +  "\",\"value\":" + String(windSpeed) +"}";
  Serial.print("Sending Wind Speed: ");
  Serial.println(windSpeedJson);

  int httpWindSpeedResponseCode = http->POST(windSpeedJson);
  Serial.print("Sent wind speed! Got Response code: ");
  Serial.println(httpWindSpeedResponseCode);
}

void loop(void)  
{  
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
        
        http.begin(client, serverName);
        http.addHeader("Content-Type", "application/json");

        sendTemperature(&http);
        sendPressure(&http);
        sendAltitude(&http);
        //sendWindSpeed(&http);
        http.end();
    } 
    else{
        Serial.println("Disconnected");
    }
    lastTime = millis();
  }
}
