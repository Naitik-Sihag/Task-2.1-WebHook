#include <SPI.h>
#include <WiFiNINA.h>
#include "DHT.h"

#define SENSOR_PIN 2
#define SENSOR_TYPE DHT11

DHT climateSensor(SENSOR_PIN, SENSOR_TYPE);

char networkSSID[] = "Naitik";
char networkPassword[] = "12345678";

const char* serverName = "api.thingspeak.com";
String writeAPIKey = "Z4XTIWXJLFSIZCNG";

WiFiClient wifiClient;

void setup() {
  Serial.begin(9600);
  delay(1000);

  climateSensor.begin();

  Serial.print("Attempting WiFi connection");
  int wifiStatus = WiFi.begin(networkSSID, networkPassword);

  while (wifiStatus != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiStatus = WiFi.status();
  }

  Serial.println("\nWiFi connected successfully!");
}

void loop() {
  float temperature = climateSensor.readTemperature();
  float humidity = climateSensor.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor.");
    delay(30000);
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("°C  Hum: ");
  Serial.print(humidity);
  Serial.println("%");

  if (wifiClient.connect(serverName, 80)) {
    String requestURL = "/update?api_key=" + writeAPIKey +
                        "&field1=" + String(temperature) +
                        "&field2=" + String(humidity);

    wifiClient.print("GET " + requestURL + " HTTP/1.1\r\n" +
                     "Host: " + serverName + "\r\n" +
                     "Connection: close\r\n\r\n");

    Serial.println("Data uploaded to ThingSpeak.");
  } else {
    Serial.println("Connection to ThingSpeak failed.");
  }

  wifiClient.stop();
  delay(30000); // Wait 30 seconds before next reading
}