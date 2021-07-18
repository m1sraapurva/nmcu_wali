
#include <FS.h>
#include <LittleFS.h>
//#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>


// Config file for reading configuration
const char* conf_file = "config.json";

struct Configuration {
    char ssid[12];
    char pass[12];
    int buzzer_mode;
    int lcd_mode;
    int led_bar_mode;
  };

Configuration conf;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
//  if(var == "GPIO_STATE"){
//    if(digitalRead(ledPin)){
//      ledState = "OFF";
//    }
//    else{
//      ledState = "ON";
//    }
//    Serial.print(ledState);
//    return ledState;
//  }
  return String();
}

void loadConfig(){
  // Read Configuration
  File confFile = LittleFS.open("config.json", "r");
  StaticJsonDocument<256> doc;
  // Deserialize the JSON Document
  DeserializationError error = deserializeJson(doc, confFile);
  if(error)
    Serial.println(F("Failed to read config. Using defaults."));  
  // Copy values to Configuration struct
  strlcpy(conf.ssid, doc["ssid"]|"unknown", sizeof(conf.ssid));
  strlcpy(conf.pass, doc["password"]|"unknown", sizeof(conf.pass));
  
  confFile.close();
}

void saveConfig(){
  
}

void setup(){

  Serial.begin(115200);

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  loadConfig();


  // Connect to Wi-Fi
  WiFi.begin(conf.ssid, conf.pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Route for configuration /config page
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/config.html", String(), false, processor);
  
  });

  // Route for configuration /config page
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
    //List all parameters
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(p->isFile()){ //p->isPost() is also true
      Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if(p->isPost()){
      Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
}
    request->send(LittleFS, "/config.html", String(), false, processor);
  
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
