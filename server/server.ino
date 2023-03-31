#include "WiFi.h"
#include "esp_wifi.h"
#include "WebServer.h"
#include <Arduino.h>

#define SWITCH_PIN 8

#define PIEZOPIN 4

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

bool switchState = true;
bool switchOnSoundBool = false;

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
    if (!switchOnSoundBool) {
      switchOnSoundBool = true;
      playSound("switchOnSound");
    }
    switchState = true;
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
  if (switch_value == LOW && switchState) {
    playSound("switchOffSound");
    switchState = false;
    switchOnSoundBool = false;
    Serial.println("Switch turned off ... closing AP");
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
          delay(1000);
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
            playSound("loosingSound");
            sequenceNeopixel("looser");
            turnOffNeopixel();
            delay(1500);
            wrongSeqDetected = true;
            // reset sequence
            seqCounter = 0;
            break;
          }
        }
        // Right Sequence
        if (!wrongSeqDetected) {
          playSound("winningSound");
          sequenceNeopixel("winner");
          turnOffNeopixel();
          delay(1500);
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
        if (i == 0) {
          pixel.setPixelColor(i, pixel.Color(0, 255, 0));
          pixel.show();
          delay(100);
        }
        else if (i == 1) {
          pixel.setPixelColor(i, pixel.Color(255, 0, 0));
          pixel.show();
          delay(100);
        }
        else if (i == 2) {
          pixel.setPixelColor(i, pixel.Color(0, 0, 255));
          pixel.show();
          delay(100);
        }
        else if (i == 3) {
          pixel.setPixelColor(i, pixel.Color(60, 255, 0));
          pixel.show();
          delay(100);
        }
        else if (i == 4) {
          pixel.setPixelColor(i, pixel.Color(0, 255, 255));
          pixel.show();
          delay(100);
        }
        else if (i == 5) {
          pixel.setPixelColor(i, pixel.Color(255, 0, 255));
          pixel.show();
          delay(100);
        }
        else if (i == 6) {
          pixel.setPixelColor(i, pixel.Color(255, 255, 0));
          pixel.show();
          delay(100);
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

void sequenceNeopixel(String color) { // IN Green / Red / Blue (not RGB)
  if (color == "empty") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(255, 255, 255));
      pixel.show();
    }
  } else if (color == "winner") {
    for (int i = 0; i < 1; i++) {
      pixel.setPixelColor(i, pixel.Color(100, 100, 100));
      pixel.show();
    }
    for (int i = 1; i < NUM_PIXELS; i++) {
      delay(100);
      pixel.setPixelColor(i, pixel.Color(255, 0, 0));
      pixel.show();
      pixel.setPixelColor(i, pixel.Color(0, 0, 0));
    }
    for (int i = 1; i < NUM_PIXELS; i++) {
      delay(100);
      pixel.setPixelColor(i, pixel.Color(255, 0, 0));
      pixel.show();
      pixel.setPixelColor(i, pixel.Color(0, 0, 0));
    }
    delay(100);
  } else if (color == "looser") {
    for (int i = 0; i < 1; i++) {
      pixel.setPixelColor(i, pixel.Color(100, 100, 100));
      pixel.show();
    }
    for (int i = 1; i < NUM_PIXELS; i++) {
      delay(100);
      pixel.setPixelColor(i, pixel.Color(0, 255, 0));
      pixel.show();
      pixel.setPixelColor(i, pixel.Color(0, 0, 0));
    }
    for (int i = 1; i < NUM_PIXELS; i++) {
      delay(100);
      pixel.setPixelColor(i, pixel.Color(0, 255, 0));
      pixel.show();
      pixel.setPixelColor(i, pixel.Color(0, 0, 0));
    }
    delay(100);
  } else if (color == "red") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(0, 255, 0));
      pixel.show();
    }
  } else if (color == "green") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(255, 0, 0));
      pixel.show();
    }
  } else if (color == "blue") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(0, 0, 255));
      pixel.show();
    }
  } else if (color == "orange") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(60, 255, 0));
      pixel.show();
    }
  } else if (color == "pink") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(0, 255, 255));
      pixel.show();
    }
  } else if (color == "turq") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(255, 0, 255));
      pixel.show();
    }
  } else if (color == "yellow") {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixel.setPixelColor(i, pixel.Color(255, 255, 0));
      pixel.show();
    }
  }
  if (color != "empty") {
    playColorSound(color);
  }
}

void playSound(String sound) {
  if (sound == "winningSound") { // F, A, C, E
    tone(PIEZOPIN, 349.23, 200); // pin, hz, duration
    delay(100);
    tone(PIEZOPIN, 440.00, 200);
    delay(100);
    tone(PIEZOPIN, 523.25, 200);
    delay(100);
    tone(PIEZOPIN, 698.46, 200);
    noTone(PIEZOPIN); 

  } else if (sound == "loosingSound") { // Bb, F, D, Bd
    tone(PIEZOPIN, 932.33, 200); // pin, hz, duration
    delay(100);
    tone(PIEZOPIN, 698.46, 200);
    delay(100);
    tone(PIEZOPIN, 587.33, 200);
    delay(100);
    tone(PIEZOPIN, 466.16, 200);
    noTone(PIEZOPIN); 

  } else if (sound == "switchOnSound") {
    tone(PIEZOPIN, 392, 100);
    tone(PIEZOPIN, 493.88, 100);
    tone(PIEZOPIN, 587.33, 150);
    delay(500);
    noTone(PIEZOPIN); 

  } else if (sound == "switchOffSound") {
    tone(PIEZOPIN, 587.33, 100);
    tone(PIEZOPIN, 493.88, 100);
    tone(PIEZOPIN, 392, 150);
    delay(500);
    noTone(PIEZOPIN); 

  } else if (sound == "connectionEstablishedSound") {
    tone(PIEZOPIN, 466.16, 100);
    tone(PIEZOPIN, 523.25, 100);
    tone(PIEZOPIN, 698.46, 150);
    delay(500);
    noTone(PIEZOPIN);
  }
}

void playColorSound(String sound) {
  if (sound == "red") { // C5
    tone(PIEZOPIN, 523.25, 1000); // pin, hz, duration
    noTone(PIEZOPIN); 

  } else if (sound == "green") { // E5
    tone(PIEZOPIN, 659.26, 1000); 
    noTone(PIEZOPIN); 
  
  } else if (sound == "blue") { // G5
    tone(PIEZOPIN, 783.99, 1000); 
    noTone(PIEZOPIN); 
  
  } else if (sound == "orange") { // B5
    tone(PIEZOPIN, 987.77, 1000); 
    noTone(PIEZOPIN); 
  
  } else if (sound == "pink") { // D5
    tone(PIEZOPIN, 587.33, 1000); 
    noTone(PIEZOPIN); 
  
  } else if (sound == "turq") { // F5
    tone(PIEZOPIN, 698.46, 1000); 
    noTone(PIEZOPIN); 
  
  } else if (sound == "yellow") { // A5
    tone(PIEZOPIN, 880.00, 1000); 
    noTone(PIEZOPIN); 
  
  } 
}

