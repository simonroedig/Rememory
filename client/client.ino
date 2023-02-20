#include "WiFi.h"

#define SWITCH_PIN 8

#include <Adafruit_NeoPixel.h>
#define NEOPIXEL_PIN 2
#define NUM_PIXELS 7
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; 

const char *ssid = "InstructorBalloon"; 
const char *password = "instructor_ap";  
bool station_started = false;

void setup() {
  Serial.begin(9600);

  pinMode(SWITCH_PIN, INPUT);
  WiFi.mode(WIFI_OFF);

}

void loop() {
  int switch_value = digitalRead(SWITCH_PIN);
  if (switch_value == HIGH) {
    stationStartedNeopixel(station_started);
    if (!station_started) {
      Serial.println("Switch turned on ... connecting to AP");
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Still connecting ...");
      }
      Serial.println("Connection to AP established");
      station_started = true;
    }
  }
  if (switch_value == LOW && station_started) {
    Serial.println("Switch turned off ... disconnecting from AP");
    WiFi.mode(WIFI_OFF); 
    station_started = false;
    turnOffNeopixel();
  }
}

void turnOffNeopixel() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, pixel.Color(0, 0, 0));
    pixel.show();
   }
}

void stationStartedNeopixel(bool stationConnected) {
  if (stationConnected) {
      for (int i = 0; i < NUM_PIXEL; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 255, 0));
        pixel.show();
      }
    return;
  }
  pixel.setPixelColor(0, pixel.Color(0, 0, 0));
  for (int i = 1; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, pixel.Color(255, 255, 255));
    pixel.show();
    delay(100);
    pixel.setPixelColor(i, pixel.Color(0, 0, 0));
  }
}