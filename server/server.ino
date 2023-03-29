#include "WiFi.h"
#include "esp_wifi.h"
#include "WebServer.h"
#include <Arduino.h>

#define SWITCH_PIN 8

#include <Adafruit_NeoPixel.h>
#define NEOPIXEL_PIN 2
#define NUM_PIXELS 7
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

#define SoundPin 4; 

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>
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

bool switchState = true;

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
    switchState = true;
    apStartedNeopixel(numStation);

    if (startGameCounter == numStation && numStation != 0) {
      game_started = true;
      turnOffNeopixel();
    }
    
    if (!ap_started) {
      Serial.println("Switch turned on ... starting AP");
      playSound("switchOnSound");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      server.begin();  // Start the server
      IPAddress IP = WiFi.softAPIP();
      Serial.print("Instructor Balloon's IP-address: ");
      Serial.println(IP);
      ap_started = true;
    }
  }
  if (switch_value == LOW && switchState) {
    switchState = false;
    Serial.println("Switch turned off ... closing AP");
    playSound("switchOffSound");
    WiFi.mode(WIFI_OFF); 
    ap_started = false;
    game_started = false;
    clientCounter = 0;
    startGameCounter = 0;
    seqStarted = false;
    seqCounter = 0;
    receivedColCounter = 0;
    wrongSeqDetected = false;
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
            playSound("losingSound");
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
          playSound("winningSound");
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
  receivedColArr[receivedColCounter] = server.arg("plain");
  server.send(200, "text/plain", "true");
  receivedColCounter++;
}

void getStartGameFromClient() {
  String received = server.arg("plain");
  Serial.println("received startgame with message: " + received);
  server.send(200, "text/plain", "true");
  startGameCounter++;
}

void getIPfromClient() {
  stationIParr[clientCounter] = server.arg("plain");
  Serial.println("IP from client is: " + stationIParr[clientCounter]);

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
void playSound(String sound){
  if (sound == "winningSound"){
    tone(4, 349.23, 200);
    delay(100 );
    tone(4, 440.00, 200);
    delay(100 );
    tone(4, 523.25 , 200);
    delay(100 );
    tone(4, 698.46 , 200);
    noTone(4); 
   } else if (sound == "losingSound"){
     tone(4, 698.46, 400);
     delay(100);
     tone(4, 523.25, 400);
     delay(100);
     tone(4, 440.00, 400);
     delay(1000);
     tone(4, 349.23, 800);
     delay(100);
     noTone(4);
   } else if (sound == "switchOnSound"){
      tone(4, 392, 100);
      tone(4, 493.88, 100);
      tone(4, 587.33, 150);
      delay(500 );
      noTone(4); 
   } else if (sound == "switchOffSound"){
      tone(4, 587.33, 100);
      tone(4, 493.88, 100);
      tone(4, 392, 150);
      delay(500 );// wait for a second
      noTone(4); 
   } else if (sound == "connectionEstablishedSound"){
      tone(4, 466.16, 100);
      tone(4, 523.25, 100);
      tone(4, 698.46, 150);
      delay(500);
      noTone(4);

 }
}
