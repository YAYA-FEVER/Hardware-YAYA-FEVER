#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//void getMode(); //request for Mode and min humidity every 5 sec
//void getExist(); //request if plant exist in the shelf every 5 sec
//void postStat(int,int,int); //post humidity(soil,air), temp to admid every hour
//void postHeight(int); //post height every week
//void Wifi_Connect();


const char* ssid = "Tong";
const char* password = "23456789";

const int plant_id = 1;
bool autoMode = false; //false = Manual , true = auto
bool isActive = false; //Is plant exist in the shelf.
int minHumid;



StaticJsonDocument<2*JSON_OBJECT_SIZE(3)> JsonMode;
StaticJsonDocument<JSON_OBJECT_SIZE(1)> JsonExist;
StaticJsonDocument<JSON_OBJECT_SIZE(4)> JsonStat;
StaticJsonDocument<JSON_OBJECT_SIZE(2)> JsonHeight;


void Wifi_Connect(){
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
      HTTPClient http;
      http.begin("https://ecourse.cpe.ku.ac.th/exceed05/api/hardware/auto_mode/1");
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
          Serial.println(minHumid);
          Serial.println(autoMode);
        }
      }else{
        Serial.println(httpCode);
        Serial.println("ERROR on HTTP Request");
      }
   }else{
    Wifi_Connect();
   }
}



void setup() {
  Serial.begin(115200);
  Wifi_Connect();
  // put your setup code here, to run once:

}

void loop() {
  getMode();
  // put your main code here, to run repeatedly:

}
