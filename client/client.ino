#include "WiFi.h"
#include <HTTPClient.h>

#define SWITCH_PIN 8

#include <Adafruit_NeoPixel.h>
#define NEOPIXEL_PIN 2
#define NUM_PIXELS 7
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; 
#define balloonPressed 1100.00

const char *ssid = "InstructorBalloon"; 
const char *password = "instructor_ap"; 
const char* serverAddressIP = "http://192.168.4.1:80/ip";
const char* serverAddressStartGame = "http://192.168.4.1:80/startGame";

bool station_started = false;
bool game_started = false;

String myColor = "empty";
bool colorReceived = false;



void setup() {
  Serial.begin(9600);

  pinMode(SWITCH_PIN, INPUT);
  WiFi.mode(WIFI_OFF);

  bmp.begin(0x76);

}

void loop() {
  int switch_value = digitalRead(SWITCH_PIN);

  if (switch_value == HIGH && !game_started) {
    stationStartedNeopixel(station_started, myColor);

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

  if (station_started) {
    if (!colorReceived) {
      HTTPClient http;
      http.begin(serverAddressIP);
      int respCode = http.POST("message=" + WiFi.localIP().toString());
      myColor = http.getString();
      http.end();
      colorReceived = true;
    }
    if (!game_started) {
      float pressure = bmp.readPressure() / 100.0F; // in hPa
      Serial.print("Current Air-Pressure: ");
      Serial.println(pressure);
      if (pressure >= balloonPressed) {
        HTTPClient http;
        http.begin(serverAddressStartGame);
        int respCode = http.POST("message=" + "1");
        game_started = http.getString(); // will return true
        http.end();
        turnOffNeopixel();
      }
    }
  }


  delay(1000);
}

void turnOffNeopixel() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, pixel.Color(0, 0, 0));
    pixel.show();
   }
}

void stationStartedNeopixel(bool stationConnected, String color) {
  if (stationConnected) {
    if (color == "empty") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 255, 255));
        pixel.show();
      }
    } else if (color == "red") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 0, 0));
        pixel.show();
      }
    } else if (color == "green") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 255, 0));
        pixel.show();
      }
    } else if (color == "blue") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 0, 255));
        pixel.show();
      }
    } else if (color == "orange") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 128, 0));
        pixel.show();
      }
    } else if (color == "pink") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 0, 255));
        pixel.show();
      }
    } else if (color == "turq") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 255, 255));
        pixel.show();
      }
    } else if (color == "yellow") {
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 255, 0));
        pixel.show();
      }
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