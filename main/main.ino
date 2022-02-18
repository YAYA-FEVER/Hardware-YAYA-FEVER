#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>


void HumidTempSoil();
void getMode(); //request for Mode and min humidity every 5 sec
void getExist(); //request if plant exist in the shelf every 5 sec
void postStat(int); //post humidity(soil,air), temp to admid every hour
void Wifi_Connect();

const char* ssid = "Mild";
const char* password = "00000000";

const int plant_id = 1;
int autoMode = 0; //false = Manual , true = auto
int isExist = 1; //Is plant exist in the shelf.
int minHumid;

// soi moisture sensor
int moisture_pin = A0;
int moisture;
char str[100];

// servo

bool isOn = false;
#define servo_pin 26

Servo servoMotor;

StaticJsonDocument<2*JSON_OBJECT_SIZE(3)> JsonMode;
StaticJsonDocument<2*JSON_OBJECT_SIZE(2)> JsonExist;
StaticJsonDocument<2*JSON_OBJECT_SIZE(3)> JsonStat;


void setup() {
  Serial.begin(115200);
  Wifi_Connect();
  servoMotor.attach(servo_pin);

  xTaskCreatePinnedToCore(HumidTempSoil,"HumidTempSoil", 32*1024, NULL, 1, NULL, 1);


}
int i=0;
void loop() {
  if(isExist==0){
    getExist();
  }else{
    getMode();
    if(autoMode==1){
       if(moisture <= minHumid) {
           servoMotor.write(180);
           vTaskDelay(3000/ portTICK_PERIOD_MS);
           servoMotor.write(90);
           vTaskDelay(5000/ portTICK_PERIOD_MS);
           servoMotor.write(0);
           vTaskDelay(3000/portTICK_PERIOD_MS);
           servoMotor.write(90);
           vTaskDelay(10000/portTICK_PERIOD_MS);
       }
    }
  }
  vTaskDelay(5000/ portTICK_PERIOD_MS);
  
}

void postStat(int soil){
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("Posting Status");
      HTTPClient http;

      http.begin("https://ecourse.cpe.ku.ac.th/exceed05/api/hardware/update/soil");
      http.addHeader("Content-Type ", "applicaiton/json");

      JsonStat["ID"] = plant_id;
      JsonStat["humidity_soil_hard"] = soil;
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

void HumidTempSoil(void* param){
    while(1){
      if(isExist==1){
        moisture = analogRead(moisture_pin);
        moisture = map(moisture,3200,600,0,100);
        //Serial.println(moisture);
        postStat(moisture);
      }
    vTaskDelay(5000/ portTICK_PERIOD_MS);
    }
}

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
      Serial.println("Getting Mode");
      HTTPClient http;
      http.begin("https://ecourse.cpe.ku.ac.th/exceed05/api/hardware/auto_mode/1");
      int httpCode = http.GET();
      if(httpCode == HTTP_CODE_OK){
        String payload = http.getString();
        DeserializationError err = deserializeJson(JsonMode, payload);
        if(err){
          Serial.print(F("deserializeJson() failed with code "));
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
   vTaskDelay(100/ portTICK_PERIOD_MS);
}

void getExist(){
   if(WiFi.status() == WL_CONNECTED){
      Serial.println("Getting Exitance");
      HTTPClient http;
      http.begin("https://ecourse.cpe.ku.ac.th/exceed05/api/hardware/exist_plant/1");
      int httpCode = http.GET();
      if(httpCode == HTTP_CODE_OK){
        String payload = http.getString();
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
