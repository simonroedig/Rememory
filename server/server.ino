#include "WiFi.h"
#include "esp_wifi.h"

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
bool ap_started = false;
bool game_started = false;

WiFiServer server(80); // Web Server on port 80

// red, orange, yellow, green, turquoise, blue, pink
int colorArr[7][3] = {{255, 0, 0}, {255, 128, 0}, {255, 255, 0}, {0, 255, 0}, {0, 255, 255}, {0, 0, 255}, {255, 0, 255}};

IPAddress stationIParr[7]; // array to store IP addresses of connected stations


void setup() {
  Serial.begin(9600);

  pinMode(SWITCH_PIN, INPUT);
  WiFi.mode(WIFI_OFF);

  bmp.begin(0x76);
}

void loop() {
  int switch_value = digitalRead(SWITCH_PIN);
  int numStation = WiFi.softAPgetStationNum();
  if (switch_value == HIGH && !game_started) {
    apStartedNeopixel(numStation);
    
    if (!ap_started) {
      Serial.println("Switch turned on ... starting AP");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      server.begin();  // Start the server
      IPAddress IP = WiFi.softAPIP();
      Serial.print("Instructor Balloon's IP-address: ");
      Serial.println(IP);
      ap_started = true;
    }
  }
  if (switch_value == LOW && ap_started) {
    Serial.println("Switch turned off ... closing AP");
    WiFi.mode(WIFI_OFF); 
    ap_started = false;
    turnOffNeopixel();
  }
  
  if (ap_started) {
    float pressure = bmp.readPressure() / 100.0F; // in hPa
    Serial.print("Current Air-Pressure: ");
    Serial.println(pressure);
    if (pressure >= 1100.00) {
      // game start
      game_started = true;
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

void apStartedNeopixel(int connectedStation) {
  if (connectedStation > 0 && connectedStation <= NUM_PIXELS) {
      for (int i = 0; i < connectedStation; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 0, 0));
        pixel.show();
        delay(100);
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
