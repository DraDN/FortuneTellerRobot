#include <SPI.h>
#include <HT_SSD1306.h>

#define BUZZER_PIN 5

#define ECHOPIN 6
#define TRIGGERPIN 7

#define PIN_RESET 9 // Connect RST to pin 9 (SPI & I2C)
#define PIN_DC 8 // Connect DC to pin 8 (SPI only)
#define PIN_CS 10 // Connect CS to pin 10 (SPI only)

SSD1306 oled(PIN_RESET, PIN_DC, PIN_CS);

struct Note {
  unsigned int frequency;
  unsigned long duration; // milliseconds
};

#define NOTES_SIZE 3
struct Note notes[NOTES_SIZE] = { { 400, 200 }, { 0, 300 }, { 600, 400 } };

float get_ultrasonic_distance() {
  digitalWrite(TRIGGERPIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIGGERPIN, HIGH);
  delayMicroseconds(50);
  digitalWrite(TRIGGERPIN, LOW);

  float duration = pulseIn(ECHOPIN, HIGH);
  return duration * 0.0344 / 2;
}

void setup() {
  Serial.begin(9600);
  oled.begin();
  oled.clear(PAGE);
  oled.clear(ALL);
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGGERPIN, OUTPUT);
}

void loop() {
  oled.clear(ALL);
  oled.setCursor(0, 1);
  oled.print("distance:");
  oled.setCursor(50, 1);
  oled.print(get_ultrasonic_distance());
  oled.display();

  Serial.print(get_ultrasonic_distance());
  Serial.print("cm,\n");
  delay(100);

  for (int i = 0; i < NOTES_SIZE; i++) {
    if (notes[i].frequency != 0) {
      tone(BUZZER_PIN, notes[i].frequency, notes[i].duration);
    }
    delay(notes[i].duration);
  }
}
