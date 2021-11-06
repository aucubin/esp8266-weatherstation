#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_SGP30.h>


const char* ssid = "ssid";
const char* password = "password";
// only connect to specified BSSID to solve problems 
// where ESP was connected to AP which was farther away
const uint8_t bssid[] = {0x30, 0x23, 0x03, 0xDD, 0xC5, 0x51};
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "mqtt_password";
const char* mqtt_broker = "example.com";
const int mqtt_port = 1883;

DHTesp dht;
Adafruit_SGP30 sgp;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, mqtt_broker, mqtt_port, mqtt_user, mqtt_password);
Adafruit_MQTT_Publish temp_publish(&mqtt, "weather/aucubin/temp");
Adafruit_MQTT_Publish tvoc_publish(&mqtt, "weather/aucubin/tvoc");
Adafruit_MQTT_Publish eco2_publish(&mqtt, "weather/aucubin/eco2");
Adafruit_MQTT_Publish humidity_publish(&mqtt, "weather/aucubin/humidity");

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void publishTemperature(){
  String temperature = String(dht.getTemperature(),3);
  int len = temperature.length()+1;
  char temperatureArray[len];
  temperature.toCharArray(temperatureArray,len);
  if(!temp_publish.publish(temperatureArray)){
    Serial.println("failed to publish temperature");
  }
  else{
    Serial.println("published temperature");
  }
}

void publishHumidity(){
  String humidity = String(dht.getHumidity(),3);
  int len = humidity.length()+1;
  char humidityArray[len];
  humidity.toCharArray(humidityArray,len);
  if(!humidity_publish.publish(humidityArray)){
    Serial.println("failed to publish humidity");
  }
  else{
    Serial.println("published humidity");
  }
}

void publishTVOC(){
  String tvoc = String(sgp.TVOC);
  int len = tvoc.length()+1;
  char tvocArray[len];
  tvoc.toCharArray(tvocArray,len);
  if(!tvoc_publish.publish(tvocArray)){
    Serial.println("failed to publish tvoc"); 
  }
  else{
    Serial.println("published tvoc");
  }
}

void publishECO2(){
  String eco2 = String(sgp.eCO2);
  int len = eco2.length()+1;
  char eco2Array[len];
  eco2.toCharArray(eco2Array,len);
  if(!eco2_publish.publish(eco2Array)){
    Serial.println("failed to publish eco2");
  }
  else{
    Serial.println("published eco2");
  }
}

void connectToWifi(){
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password, 0, bssid);

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTT(){
  Serial.println("Connecting to MQTT...");
  while(mqtt.connect() != 0){
    delay(1000);
    Serial.print(".");
    mqtt.disconnect();
  }
  Serial.println("MQTT connected");
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

  connectToWifi();  
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    connectToWifi();
  }
  else if(!mqtt.connected()){
    connectToMQTT();
  }
  sgp.setHumidity(getAbsoluteHumidity(dht.getTemperature(),dht.getHumidity()));
  sgp.IAQmeasure();
  Serial.println("Start publishing data...");
  publishTemperature();
  publishHumidity();
  publishTVOC();
  publishECO2();
  delay(10000);
}
