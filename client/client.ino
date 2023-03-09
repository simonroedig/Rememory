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
#define balloonPressed 1000.00

const char *ssid = "InstructorBalloon"; 
const char *password = "instructor_ap"; 
const char* serverAddressIP = "http://192.168.4.1:80/ip";
const char* serverAddressStartGame = "http://192.168.4.1:80/startGame";
const char* serverAddressSendCol = "http://192.168.4.1:80/sendCol";

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
    colorReceived = false;
    game_started = false;
    turnOffNeopixel();
  }

  if (station_started) {
    if (!colorReceived) {
      HTTPClient http;
      http.begin(serverAddressIP);
      http.addHeader("Content-Type", "text/plain");
      Serial.println("The IP-message is: " +  WiFi.localIP().toString());
      int respCode = http.POST(WiFi.localIP().toString());
      myColor = http.getString();
      Serial.println(myColor);
      http.end();
      colorReceived = true;
      Serial.println("Color Received");
    }
    if (!game_started) {
      float pressure = bmp.readPressure() / 100.0F; // in hPa
      Serial.print("Current Air-Pressure: ");
      Serial.println(pressure);
      if (pressure >= balloonPressed) {
        Serial.println("Pressure over Treshhold, Start Game will sent to server");
        HTTPClient http2;
        http2.begin(serverAddressStartGame);
        http2.addHeader("Content-Type", "text/plain");
        int respCode = http2.POST("1");
        String respBody = http2.getString(); // will return true
        Serial.println(respBody);
        if (respBody == "true") {
          Serial.println("Start game was sent to server and received true");
          game_started = true;
          http2.end();
        } else {
          Serial.println("in game start func, false zweig");
          return;
        }
        middleNeopixelMode(station_started, myColor);
      }
    }
    // main game starts here
    if (colorReceived && game_started) {
      float pressure = bmp.readPressure() / 100.0F; // in hPa
      Serial.print("Current Air-Pressure: ");
      Serial.println(pressure);
      if (pressure >= balloonPressed) {

        stationStartedNeopixel(station_started, myColor);
        
        HTTPClient http3;
        http3.begin(serverAddressSendCol);
        http3.addHeader("Content-Type", "text/plain");
        int respCode = http3.POST(myColor);
        String respBody = http3.getString(); 
        http3.end();

        delay(1500);
        middleNeopixelMode(station_started, myColor);
      }
    }
  }


  delay(500);
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

// only lights the middle neopixel
void middleNeopixelMode(bool stationConnected, String color) {
  turnOffNeopixel();
  if (stationConnected) {
    if (color == "empty") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 255, 255));
        pixel.show();
      }
    } else if (color == "red") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 0, 0));
        pixel.show();
      }
    } else if (color == "green") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 255, 0));
        pixel.show();
      }
    } else if (color == "blue") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 0, 255));
        pixel.show();
      }
    } else if (color == "orange") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 128, 0));
        pixel.show();
      }
    } else if (color == "pink") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 0, 255));
        pixel.show();
      }
    } else if (color == "turq") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 255, 255));
        pixel.show();
      }
    } else if (color == "yellow") {
      for (int i = 0; i < 1; i++) {
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
