void makeTone(int note) {
  tone(BUZZER_PIN, note, 3000);
  noTone(BUZZER_PIN);
}

void stopTone() {
  noTone(BUZZER_PIN);
}