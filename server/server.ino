#include "WiFi.h"
#include "esp_wifi.h"
#include "WebServer.h"
#include <Arduino.h>

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

WebServer server(80); // 80 = port

// red, orange, yellow, green, turquoise, blue, pink (just for color code in rgb)
// colors: {{255, 0, 0}, {255, 128, 0}, {255, 255, 0}, {0, 255, 0}, {0, 255, 255}, {0, 0, 255}, {255, 0, 255}};
String colorStringArr[7] = {"red", "green", "blue", "orange", "pink", "turq", "yellow"};

String stationIParr[7]; // array to store IP addresses of connected stations
int clientCounter = 0;
int startGameCounter = 0;

String availableColorsArr[7];
String gameSequenceArr[50];
bool seqStarted = false;
int seqCounter = 0;
String receivedColArr[50];
int receivedColCounter = 0;
bool wrongSeqDetected = false;

void setup() {
  Serial.begin(9600);

  pinMode(SWITCH_PIN, INPUT);
  
  WiFi.mode(WIFI_OFF);

  bmp.begin(0x76);

  server.on("/ip", HTTP_POST, getIPfromClient);
  server.on("/startGame", HTTP_POST, getStartGameFromClient);
  server.on("/sendCol", HTTP_POST, receiveColor);
  randomSeed(analogRead(0));
}

void loop() {
  int switch_value = digitalRead(SWITCH_PIN);
  int numStation = WiFi.softAPgetStationNum();

  if (switch_value == HIGH && !game_started) {
    apStartedNeopixel(numStation);

    if (startGameCounter == numStation && numStation != 0) {
      game_started = true;
      turnOffNeopixel();
    }
    
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
    game_started = false;
    turnOffNeopixel();
  }
  
  // main game starts here
  if (ap_started) {
    if (game_started) {
      if (!seqStarted) {
        delay(2500);
        //start sequence
        for (int i = 0; i < numStation; i++) {
          availableColorsArr[i] = colorStringArr[i];
        }
        int randNum = random(numStation); // ran between 0 and numStation-1
        gameSequenceArr[seqCounter] = availableColorsArr[randNum];

        for (int i = 0; i <= seqCounter; i++) {
          sequenceNeopixel(gameSequenceArr[i]);
          delay(1500);
        }
        turnOffNeopixel();

        seqCounter++;
        seqStarted = true;
      }
      // check if received sequence is correct
      if (seqStarted && receivedColCounter == seqCounter) {
        for (int i = 0; i <= seqCounter - 1; i++) {
          // Wrong Sequence
          if (gameSequenceArr[i] != receivedColArr[i]) {
            sequenceNeopixel("red");
            delay(500);
            turnOffNeopixel();
            delay(500);
            sequenceNeopixel("red");
            delay(500);
            turnOffNeopixel();
            delay(500);
            sequenceNeopixel("red");
            delay(500);
            turnOffNeopixel();
            wrongSeqDetected = true;
            // reset sequence
            seqCounter = 0;
            break;
          }
        }
        // Right Sequence
        if (!wrongSeqDetected) {
          sequenceNeopixel("green");
          delay(500);
          turnOffNeopixel();
          delay(500);
          sequenceNeopixel("green");
          delay(500);
          turnOffNeopixel();
          delay(500);
          sequenceNeopixel("green");
          delay(500);
          turnOffNeopixel();
        }
        seqStarted = false;
        receivedColCounter = 0;
        wrongSeqDetected = false;
      }
    }
  }



  server.handleClient();
  delay(500);
}

void receiveColor() {
  receivedColArr[receivedColCounter] = server.arg("message");
  server.send(200, "text/plain", "true");
  receivedColCounter++;
}

void getStartGameFromClient() {
  String received = server.arg("message");
  server.send(200, "text/plain", "true");
  startGameCounter++;
}

void getIPfromClient() {
  stationIParr[clientCounter] = server.arg("message");

  for (int i = 0; i < 7; i++) {
    Serial.println(stationIParr[i]);
  }
  // respond with color
  server.send(200, "text/plain", colorStringArr[clientCounter]);
  clientCounter++;
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
        pixel.setPixelColor(i, pixel.Color(255, 255, 255));
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

void sequenceNeopixel(String color) {
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
}

