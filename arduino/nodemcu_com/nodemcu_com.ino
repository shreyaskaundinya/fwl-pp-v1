#include <Wire.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "N8B";
const char* password = "12345678";

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://192.168.193.6:5000/upload";


void setup()
{ 
  Serial.begin (9600); 
  WiFi.begin(ssid, password);
  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Trying again...");
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Wire.begin(D1, D2);
}


void loop()
{ 
  Serial.println("Requesting...");
  Wire.requestFrom(8, 3); 
  char sensorState[3];
  int i = 0;
  
  while(Wire.available()){ 
    char c = Wire.read();
    sensorState[i] = c;
    Serial.print(c);
    i++;
  }

  String rfidState, laserState, pressureState;

  if (sensorState[0] == '1') rfidState = "true";
  else rfidState = "false";

  if (sensorState[1] == '1') laserState = "true";
  else laserState = "false";

  if (sensorState[2] == '1') pressureState = "true";
  else pressureState = "false";

  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/json");
    // String requestData = "{\"rfid\":true,\"laser\":true,\"force\":true,\"unlocked\":false}";
    String message = "{\"rfid\":\"" + rfidState + "\",\"laser\":\"" + laserState + "\",\"force\":\"" + pressureState + "\",\"unlocked\":\"" + rfidState + "\"}";
    Serial.println(message);
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(message);
   
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  
  delay(4000); 
}
