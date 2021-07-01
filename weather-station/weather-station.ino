#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

DHTesp dht;
ESP8266WebServer server(80);

const char* ssid = "ssid";
const char* password = "password";


void getTemperature(){
  server.send(200, "text/plain", String(dht.getTemperature(),3));
}

void getHumidity(){
  server.send(200, "text/plain", String(dht.getHumidity(),3));
}

void notFound(){
  server.send(400, "text/plain", "404: Not found");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  dht.setup(D6, DHTesp::AM2302);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/temp", getTemperature);
  server.on("/humidity", getHumidity);
  server.onNotFound(notFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
