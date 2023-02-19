float getAirPressure() {
  float pressure = bmp.readPressure() / 100.0F; 
  return pressure;
}
