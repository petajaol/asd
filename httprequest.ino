#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
WiFiClient wificlient;
int flag = 0;

void setup() {
  Serial.begin(115200);          
  WiFi.begin("user", "password"); //muodostetaan wifi-yhteys
  while (WiFi.status() != WL_CONNECTED) {  //tarkastetaan onko yhteys muodostettu
  delay(500);
  }

}

void loop() {

  if (WiFi.status() == WL_CONNECTED && flag == 0) { //jos yhteys muodostettu, suoritetaan HTTP-pyyntö
    HTTPClient http;    
    http.begin(wificlient, "http://maker.ifttt.com/trigger/halytys123/with/key/kyAYZHHTgSkiv1BXx_SNJQ--IwjS_s8x_Iu-rhTQbDp");
    http.addHeader("Content-Type", "text/plain"); 
    int httpCode = http.POST("Message from ESP8266");   
    String payload = http.getString();                  
    http.end();
    flag = 1; //jotta pyyntö tehtäisiin vain kerran laitteen päälläolon aikana (uusi pyyntö voidaan tuottaa resettaamalla laite)
  }
}
