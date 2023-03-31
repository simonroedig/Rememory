#include "WiFi.h"
#include <HTTPClient.h>

#define SWITCH_PIN 8
#define PIEZOPIN 4

#include <Adafruit_NeoPixel.h>
#define NEOPIXEL_PIN 2
#define NUM_PIXELS 7
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bmp; 
float pressure = 1000.00;

String myColor = "green";

bool switchState = true;
bool switchOnSoundBool = false;

void setup() {
  Serial.begin(9600);

  pinMode(SWITCH_PIN, INPUT);
  WiFi.mode(WIFI_OFF);

  bmp.begin(0x76);
}

void loop() {
  int switch_value = digitalRead(SWITCH_PIN);

  if (switch_value == HIGH) {
    if (!switchOnSoundBool) {
      pressure = bmp.readPressure() / 100.0F;
      switchOnSoundBool = true;
      playSound("switchOnSound");
    }
    switchState = true;

    stationStartedNeopixel(false, "empty");
    stationStartedNeopixel(false, "empty");
    stationStartedNeopixel(false, "empty");
    stationStartedNeopixel(false, "empty");
    stationStartedNeopixel(false, "empty");
    stationStartedNeopixel(false, "empty");
    stationStartedNeopixel(false, "empty");
    stationStartedNeopixel(false, "empty");
    playSound("connectionEstablishedSound");
    stationStartedNeopixel(true, "empty");
    delay(3000);
    stationStartedNeopixel(true, myColor);
    delay(8000);
    
  }

  if (switch_value == LOW && switchState) {
    playSound("switchOffSound");
    switchState = false;
    switchOnSoundBool = false;
    turnOffNeopixel();
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
  if (stationConnected) {
    if (color == "empty") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 255, 255));
        pixel.show();
      }
    } else if (color == "red") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 255, 0));
        pixel.show();
      }
    } else if (color == "green") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 0, 0));
        pixel.show();
      }
    } else if (color == "blue") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 0, 255));
        pixel.show();
      }
    } else if (color == "orange") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(60, 255, 0));
        pixel.show();
      }
    } else if (color == "pink") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 255, 255));
        pixel.show();
      }
    } else if (color == "turq") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 0, 255));
        pixel.show();
      }
    } else if (color == "yellow") {
      for (int i = 0; i < 1; i++) {
        pixel.setPixelColor(i, pixel.Color(255, 255, 0));
        pixel.show();
      }
    }
    for (int i = 1; i < NUM_PIXELS; i++) {
        pixel.setPixelColor(i, pixel.Color(0, 0, 0));
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

