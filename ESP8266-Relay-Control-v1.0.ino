/*
 ESP8266 Relay Control v1.0
 Project to attach 4 relay via web or via mobile application.
 Copyright (c) 2012-2017 CNSoluciones   
 Created 24 Jun 2017 - By Federico Pereira <fpereira@cnsoluciones.com>
 License: Apache License - https://www.apache.org/licenses/LICENSE-2.0
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "**********";
const char* password = "*****";

ESP8266WebServer server(80);

//http://www.kloppenborg.net/images/blog/esp8266/esp8266-node-mcu-pinout.png
String version = "1.0";

const int relay1 = 12;  //D6
const int relay2 = 13;  //D7
const int relay3 = 14;  //D5
const int relay4 = 15;  //D8

void handleRoot() {
  
  String Active1 = "";
  String Active2 = "";
  String Active3 = "";
  String Active4 = "";
  
  if( String(digitalRead(relay1)) == "1" ) Active1 = "checked";
  if( String(digitalRead(relay2)) == "1" ) Active2 = "checked";
  if( String(digitalRead(relay3)) == "1" ) Active3 = "checked";
  if( String(digitalRead(relay4)) == "1" ) Active4 = "checked";

  String webPage = 
"<!DOCTYPE html>\r\n\
<html lang='en'>\r\n\
<head>\r\n\
  <title>ESP8266 Relay Control v"+version+"</title>\r\n\
  <meta charset='utf-8'>\r\n\
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\r\n\
  <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>\r\n\
  <link href='https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css' rel='stylesheet'>\r\n\
  <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>\r\n\
  <script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>\r\n\
  <script src='https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js'></script>\r\n\
 </head>\r\n\
<body>\r\n\
<div class='container-fluid'>\r\n\
  <div class='page-header text-cente'>\r\n\
    <h1>ESP8266 Relay Control v"+version+"</h1>\r\n\
  </div>\r\n\
<div class='row form-group'>\r\n\
  <div class='col-xs-12 col-sm-3 form-group'>\r\n\
  <label class='checkbox-inline'>Relay 1&nbsp;&nbsp;</label>\r\n\
  <input id='relay' name='relay1' type='checkbox' "+Active1+" data-toggle='toggle' onchange='relayAction(this,name)'></div>\r\n\
  <div class='col-xs-12 col-sm-3 form-group'>\r\n\
  <label class='checkbox-inline'>Relay 2&nbsp;&nbsp;</label>\r\n\
  <input id='relay' name='relay2' type='checkbox' "+Active2+" data-toggle='toggle' onchange='relayAction(this,name)'></div>\r\n\
  <div class='clearfix visible-xs'></div>\r\n\
  <div class='col-xs-12 col-sm-3 form-group'>\r\n\
  <label class='checkbox-inline'>Relay 3&nbsp;&nbsp;</label>\r\n\
  <input id='relay' name='relay3' type='checkbox' "+Active3+" data-toggle='toggle' onchange='relayAction(this,name)'></div>\r\n\
  <div class='col-xs-12 col-sm-3 form-group'>\r\n\
  <label class='checkbox-inline'>Relay 4&nbsp;&nbsp;</label>\r\n\
  <input id='relay' name='relay4' type='checkbox' "+Active4+" data-toggle='toggle' onchange='relayAction(this,name)'></div>\r\n\
</div>\r\n\
</div>\r\n\
<script>\r\n\
  $(function() {\r\n\
    $('#relay').bootstrapToggle();\r\n\
  })\r\n\
  \r\n\
  function relayAction(el, name){\r\n\
    if(el.checked){\r\n\
      //console.log('on  - '+name);\r\n\
      var url = '/'+name+'?state=on';\r\n\
    }else{\r\n\
      //console.log('off - '+name);\r\n\
      var url = '/'+name+'?state=off';\r\n\
    }\r\n\
    //console.log(url);\r\n\
    var xhReq = new XMLHttpRequest();\r\n\
    xhReq.open('GET', url, true);\r\n\
    xhReq.setRequestHeader('Content-type', 'application/json');\r\n\
    xhReq.send();\r\n\
    \r\n\
    xhReq.onreadystatechange = function () {\r\n\
      if (xhReq.readyState == 4 && xhReq.status == 200) {\r\n\
        console.log(xhReq.responseText);\r\n\
      }\r\n\
    }\r\n\
  }\r\n\
</script>\r\n\
</body>\r\n\
</html>";

 
  server.send(200, "text/html", webPage);

}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void setup(void){
  WiFi.hostname("PredatorFanboy");
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/version", [](){
    server.send ( 200, "application/json", "{'version':'"+ version +"'}" );
  });

  server.on("/relay1", []() {
    String state=server.arg("state");
    if (state == "on") digitalWrite(relay1, HIGH);
    else if (state == "off") digitalWrite(relay1, LOW);
    server.send ( 200, "application/json", "{'1': {'status':'"+ state +"'}}" );
    Serial.println("Relay1: "+state);
  });

  server.on("/relay2", []() {
    String state=server.arg("state");
    if (state == "on") digitalWrite(relay2, HIGH);
    else if (state == "off") digitalWrite(relay2, LOW);
    server.send ( 200, "application/json", "{'2': {'status':'"+ state +"'}}" );
    Serial.println("Relay2: "+state);
  });

  server.on("/relay3", []() {
    String state=server.arg("state");
    if (state == "on") digitalWrite(relay3, HIGH);
    else if (state == "off") digitalWrite(relay3, LOW);
    server.send ( 200, "application/json", "{'3': {'status':'"+ state +"'}}" );
    Serial.println("Relay3: "+state);
  });

  server.on("/relay4", []() {
    String state=server.arg("state");
    if (state == "on") digitalWrite(relay4, HIGH);
    else if (state == "off") digitalWrite(relay4, LOW);
    server.send ( 200, "application/json", "{'4': {'status':'"+ state +"'}}" );
    Serial.println("Relay4: "+state);
  });

 server.on("/getstatus", []() {
    String relay=server.arg("relay");
    if (relay == "1")   
      server.send ( 200, "application/json", "{'1': {'status': '"+String(digitalRead(relay1))+"'}}");
    else if (relay == "2") 
      server.send ( 200, "application/json", "{'2': {'status': '"+String(digitalRead(relay2))+"'}}");
    else if (relay == "3")
      server.send ( 200, "application/json", "{'3': {'status': '"+String(digitalRead(relay3))+"'}}");
    else if (relay == "4")
      server.send ( 200, "application/json", "{'4': {'status': '"+String(digitalRead(relay4))+"'}}"); 
    else
      handleNotFound();
  });

  server.on("/help", [](){
    String webhelp = "<meta http-equiv='refresh' content='0; url=https://github.com/lordbasex/ESP8266-Relay-Control-v1.0' />";
    server.send(200, "text/html", webhelp);
  });
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
