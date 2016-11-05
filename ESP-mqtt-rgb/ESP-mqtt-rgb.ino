#include <Arduino.h>
#include <CMMC_Manager.h>
#include "CMMC_Interval.hpp"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <MqttConnector.h>
#include "init_mqtt.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN         13
#define NUMPIXELS   1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int weight;

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

#include "_publish.h"
#include "_receive.h"

const char* MQTT_HOST        = "mqtt.espert.io";
const char* MQTT_USERNAME    = "";
const char* MQTT_PASSWORD    = "";
const char* MQTT_CLIENT_ID   = "";
const char* MQTT_PREFIX      = "/CMMC";
const int MQTT_PORT           = 1883;
const int PUBLISH_EVERY       = 2000;

/* DEVICE DATA & FREQUENCY */
const char *DEVICE_NAME = "CMMC-ROOM-RGB";

#define BUTTON_INPUT_PIN 0
CMMC_Manager manager(BUTTON_INPUT_PIN, LED_BUILTIN);

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#define DHTPIN 12     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

CMMC_Interval interval;

MqttConnector *mqtt;

float t_dht = 0;
float h_dht = 0;

static void read_dht() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    t_dht = t;
    h_dht = h;
    Serial.print("Temp: ");
    Serial.println(t_dht);
    Serial.print("Humid: ");
    Serial.println(h_dht);
  }
}

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(200);
  dht.begin();
  pixels.begin();
}

void setup()
{
  init_hardware();
  manager.start();
  init_mqtt();
}

void loop()
{
  mqtt->loop();
  if (analogRead(A0) > 100)  {
    rainbow(10);
  } else {
    pixels.setPixelColor(0, pixels.Color(128, 128, 128));
    pixels.show();
  }

  Serial.println(analogRead(A0));
  
}
