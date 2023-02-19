#define SWITCH_PIN 8
#define BUZZER_PIN 4

#include <Adafruit_NeoPixel.h>
#define NEOPIXEL_PIN 2
#define NUM_PIXELS 7
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; 

void setup() {
  Serial.begin(9600);
  pinMode(SWITCH_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  bmp.begin(0x76);
}

void loop() {
  if(digitalRead(SWITCH_PIN) == HIGH) {
    Serial.print("aergfnazerguzaeb");
    stopTone();
    lightStart();
    
  }
  else if(digitalRead(SWITCH_PIN) == LOW) {
    lightBlue();
    makeTone(200);
    Serial.print(getAirPressure());
  }
  delay(300);
}
