void lightGreen() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, pixel.Color(0, 255, 0));
    pixel.show();
   }
}

void lightRed() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, pixel.Color(255, 0, 0));
    pixel.show();
   }
}

void lightBlue() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, pixel.Color(0, 0, 255));
    pixel.show();
   }
}

void lightStart() {
  pixel.setPixelColor(0, pixel.Color(0, 0, 0));
  for (int i = 1; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, pixel.Color(255, 255, 255));
    pixel.show();
    delay(200);
    pixel.setPixelColor(i, pixel.Color(0, 0, 0));
   }
}