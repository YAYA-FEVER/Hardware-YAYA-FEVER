#include "Ultrasonic.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
// air humid and temp
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// ultrasonic
Ultrasonic ultrasonic(5);

// Do Height task only (try with humidtemphumid at 15.22 18 feb)

long RangeInCentimeters;
long MAX_height = 30;
long height;

void getMode(); //request for Mode and min humidity every 5 sec
void getExist(); //request if plant exist in the shelf every 5 sec
void postStat(float,float); //post humidity(air), temp to admid every hour
void postHeight(); //post height every week
void Wifi_Connect();

const char* ssid = "TheBenz2";
const char* password = "11111111";
char str[50];
const int plant_id = 1;
int isExist = 0; //Is plant exist in the shelf. 0 no 1 yes
int minHumid;
const int ID = 1;
StaticJsonDocument<2*JSON_OBJECT_SIZE(2)> JsonMode;
StaticJsonDocument<2*JSON_OBJECT_SIZE(1)> JsonExist;
StaticJsonDocument<2*JSON_OBJECT_SIZE(4)> JsonStat;
StaticJsonDocument<2*JSON_OBJECT_SIZE(3)> JsonHeight;

// work in case that plant is a the center of sensor
void setup() {
  Serial.begin(115200);
  Wifi_Connect();
  dht.begin();
  xTaskCreatePinnedToCore(HumidTemp,"HumidTemp", 32*1024, NULL, 1, NULL, 1);
  
} 
void loop() {
  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  height = MAX_height - RangeInCentimeters;
  Serial.println(height);
  postHeight();
  delay(5000);
}

void HumidTemp(void* param){
    while(1){
    int val = analogRead(A0);

    float humi  = dht.readHumidity();
    float tempC = dht.readTemperature();
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
    Serial.println("°C ~ ");
    postStat(humi, tempC);
  }

    vTaskDelay(2000/ portTICK_PERIOD_MS);
    }
}

void Wifi_Connect(){
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
    Serial.println("Connecting to Wifi network");
    Serial.print("IP Address : ");
    // check internet connection
    Serial.println(WiFi.localIP());
}

void postHeight(){
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    http.begin("https://ecourse.cpe.ku.ac.th/exceed05/api/hardware/update_height");
    http.addHeader("Content-Type ", "applicaiton/json");


    JsonHeight["ID"] = plant_id;
    JsonHeight["height_hard"] = height;
    serializeJson(JsonHeight, str);
    int httpCode = http.POST(str);

    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

    } else{
      Serial.println(httpCode);
      Serial.println("Error On HTTP Code");
    }
  } else{
    Wifi_Connect();
  }
  delay(100);
}
void getExist(){
   if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
      http.begin("https://ecourse.cpe.ku.ac.th/exceed05/api/hardware/exist_plant/1");
      int httpCode = http.GET();
      if(httpCode == HTTP_CODE_OK){
        String payload = http.getString();
        // plant is on the shelf
        isExist = 1;
        DeserializationError err = deserializeJson(JsonExist, payload);
        if(err){
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(err.c_str());
        }else{
          Serial.println(httpCode);
          isExist=JsonMode["existed"];
          Serial.println(isExist);
        }
      }else{
        Serial.println(httpCode);
        Serial.println("ERROR on HTTP Request");
      }
   }else{
    Wifi_Connect();
   }
}
void postStat(float humi, float temp){
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("Posting Status");
      HTTPClient http;

      http.begin("https://ecourse.cpe.ku.ac.th/exceed05/api/hardware/update/air");
      http.addHeader("Content-Type ", "applicaiton/json");

      JsonStat["ID"] = plant_id;;
      JsonStat["humidity_air_hard"] = humi;
      JsonStat["temp"] = temp;
      serializeJson(JsonStat, str);
      int httpCode = http.POST(str);

      if(httpCode == HTTP_CODE_OK){
       String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);

      } else{
        Serial.println(httpCode);
        Serial.println("Error On HTTP Code");
      }
    } else{
      Wifi_Connect();
  }
  vTaskDelay(100/ portTICK_PERIOD_MS);
}
