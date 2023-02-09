#include <Adafruit_NeoPixel.h>

// GPIO 2 = A0 = D0
#define PIN 2
#define NUMPIXELS 7

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  pixels.begin();
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    Serial.print("Pixel set to 0, 0, 150 \n");
    pixels.show();
    delay(200);
   }
   for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    Serial.print("Pixel set to 0, 0, 150 \n");
    pixels.show();
    delay(200);
  }
   for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0,255, 0));
    Serial.print("Pixel set to 0, 0, 150 \n");
    pixels.show();
    delay(200);
  }
}