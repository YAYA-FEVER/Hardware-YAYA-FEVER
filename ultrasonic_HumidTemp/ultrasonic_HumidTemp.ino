#include "Ultrasonic.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Ultrasonic ultrasonic(5);

long RangeInCentimeters;
long MAX_height = 30;
long height;

// work in case that plant is a the center of sensor
void setup() {
  Serial.begin(115200);
  dht.begin();
  xTaskCreatePinnedToCore(HumidTemp,"HumidTemp", 3*1024, NULL, 1, NULL, 1);
}

void loop() {
  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  height = MAX_height - RangeInCentimeters;
  Serial.println(height);
  delay(100);
}

void HumidTemp(void* param){
    while(1){
    int val = analogRead(A0);
//   Serial.(val);
    float humi  = dht.readHumidity();
    // read temperature as Celsius
    float tempC = dht.readTemperature();
    // read temperature as Fahrenheit
    float tempF = dht.readTemperature(true);
    if (isnan(humi) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  "); 

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C ~ ");
    Serial.print(tempF);
    Serial.println("°F");
  }
    vTaskDelay(2000/ portTICK_PERIOD_MS);
    }
}
