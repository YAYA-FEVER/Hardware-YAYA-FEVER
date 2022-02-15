#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

void getMode(); //request for Mode and min humidity every 5 sec
void getExist(); //request if plant exist in the shelf every 5 sec
void postStat(int,int,int); //post humidity(soil,air), temp to admid every hour
void postHeight(int); //post height every week
void Wifi_Connect();

const char* ssid = "";
const char* password = "";

const plant_id = 1;
bool autoMode = false; //false = Manual , true = auto
bool isActive = false; //Is plant exist in the shelf.
int minHumid;

StaticJsonDocument<JSON_OBJECT_SIZE(2)> JsonMode;
StaticJsonDocument<JSON_OBJECT_SIZE(1)> JsonExist;
StaticJsonDocument<JSON_OBJECT_SIZE(4)> JsonStat;
StaticJsonDocument<JSON_OBJECT_SIZE(2)> JsonHeight

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}


void WiFi_Connect(){
  WiFi.disconnect();
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
}


void getMode(){
   if(WiFi.status() == WL_CONNECTED){
      http.begin(...); // No get url yet!!!!
      int httpCode = http.GET();
      if(httpCode == HTTP_CODE_OK){
        String payload = http.getString();
        DeserializationError err = deserializeJson(JsonMode, payload);
        if(err){
          Serial.print(F("deserializeJson() failed with code"));
          Serial.println(err.c_str());
        }else{
          Serial.println(httpCode);
          minHumid=JsonMode["humidity_soil_front"];
          autoMode=JsonMode["activate_auto"];
        }
      }else{
        Serial.println(httpCode);
        Serial.println("ERROR on HTTP Request");
      }
   }else{
    WiFi_Connect();
   }
}
