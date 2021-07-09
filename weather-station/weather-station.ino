#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_SGP30.h>

DHTesp dht;
ESP8266WebServer server(80);
Adafruit_SGP30 sgp;

const char* ssid = "ssid";
const char* password = "password";

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void getTemperature(){
  server.send(200, "text/plain", String(dht.getTemperature(),3));
}

void getHumidity(){
  server.send(200, "text/plain", String(dht.getHumidity(),3));
}

void getTVOC(){
  
  sgp.setHumidity(getAbsoluteHumidity(dht.getTemperature(),dht.getHumidity()));

  if(!sgp.IAQmeasure()){
    server.send(500, "text/plain", String("Measure failed"));
  }
  else{
    server.send(200, "text/plain", String(sgp.TVOC));
  }
}

void getECO2(){
  sgp.setHumidity(getAbsoluteHumidity(dht.getTemperature(),dht.getHumidity()));

  if(!sgp.IAQmeasure()){
    server.send(500, "text/plain", String("Measure failed"));
  }
  else{
    server.send(200, "text/plain", String(sgp.eCO2));
  }
}

void notFound(){
  server.send(400, "text/plain", "404: Not found");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  Serial.println("Initializing DHT...");
  dht.setup(D6, DHTesp::AM2302);
  Serial.println("Initializing SGP...");
  if(!sgp.begin()){
    Serial.println("SGP not found...");
    while(1);
  }

  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  sgp.setIAQBaseline(0x8FE3, 0x920C);

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
  server.on("/tvoc", getTVOC);
  server.on("/eco2", getECO2);
  server.onNotFound(notFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
