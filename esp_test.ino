#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"

#define RELAY_NO true //Normally Open Configuration for Relay
#define num_relays 4

int relayGPIOS[num_relays] = {};

const char* ssid = "GALAXY A318B56";
const char* password = "sudarshbuxyyes";

const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";

AsyncWebServer server(80);//Creates AsyncWebServer object on port 80

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name = "viewport" content = "width=device-width, initial-scale = 1">
  <style>
      html {font-family:Arial, display:inline-block, text-align:center;}
      h2 {font-size: 3.0rem;}
      p {font-size: 3.0rem;}
      body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
      .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
      .switch input {display: none}
      .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
      .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
      input:checked+.slider {background-color: #2196F3}
      input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
   </style>
</head>
<body>
  <h2> ESP Web Server </h2>
  %BUTTONPLACEHOLDER%
  <script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}</script>
</body>
</head>
)rawliteral";


String processor(const String& var){
  if(var=="BUTTONPLACEHOLDER"){
    String buttons = "";
    for(int i = 0; i < num_relays; i++){
      String relayStateValue = relayState(i+1);
      buttons += "<h4>Relay No." + String(i+1) + " -GPIO " + relayGPIOS[i];
      
      
    }
    return buttons;
  }
  return String();
  
}

String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOS[numRelay])){
      return "";
    } else{
      return "checked";
    }
  } else{
    if(digitalRead(relayGPIOS[numRelay])){
      return "checked";
      
    } else{
      return "";
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(int i=1;i<=num_relays;i++){
    pinMode(relayGPIOS[i-1],OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOS[i-1],HIGH);
      
    }else{
      digitalWrite(relayGPIOS[i-1],LOW);
    }
  }
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      if(RELAY_NO){
        Serial.print("NO ");
        digitalWrite(relayGPIOS[inputMessage.toInt()-1], !inputMessage2.toInt());
      }
      else{
        Serial.print("NC ");
        digitalWrite(relayGPIOS[inputMessage.toInt()-1], inputMessage2.toInt());
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}
