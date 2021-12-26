# ESP8266 Weather Station
Simple Weather Station based on ESP8266 that publishes data via WiFi to a MQTT broker

# Required parts
- ESP8266
- DHT21 Temperature + Humidity Sensor
- Adafruit SGP30 CO2 Sensor

# Building
- Install platformio extension for Visual Studio Code
- Copy creds.h.template to creds.h and enter credentials for WiFi and MQTT Broker
- Adjust MQTT topics
- Check that the pins defined in the main.cpp match the pins where the sensors are connected on your ESP8266
- Adjust the Base line for the SGP30
- Build and flash onto your ESP8266