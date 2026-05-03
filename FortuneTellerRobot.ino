#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/// PINS

#define BUZZER_PIN 5

#define ECHOPIN 6
#define TRIGGERPIN 7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   11 // D1
#define OLED_CLK    13 // D0
#define OLED_DC      8
#define OLED_CS     10
#define OLED_RESET   9

/// VARIABLES

#define HAND_DISTANCE_THRESHOLD 6.0
#define NO_ROUNDS 2
#define QUOTE_DISPLAY_TIME 3200; // milliseconds
int tempo = 144 * 2;
#define HAND_TIME_BUFFER 5

/// TIME DELTA

unsigned int LAST_TIME = 0;
unsigned int DELTA_T = 0;

void update_delta_t() {
  DELTA_T = millis() - LAST_TIME;
  LAST_TIME = millis();
}

short hand_time = 0;

/// SCREEN

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

void centeredWordWrap(const char* text, int startY = 8, int fontSize = 1) {
  int maxChars = SCREEN_WIDTH / (6 * fontSize);
  int lineHeight = 8 * fontSize;
  int y = startY;
  
  const char* ptr = text;
  oled.setTextSize(fontSize);
  oled.setTextColor(SSD1306_WHITE);

  while (*ptr != '\0') {
    char lineBuffer[22]; // Slightly larger than maxChars for safety
    int charsInLine = 0;
    
    // Find how many characters fit on this line
    const char* endPtr = ptr;
    int lastSpaceIdx = -1;
    
    for (int i = 0; i < maxChars && endPtr[i] != '\0'; i++) {
      if (endPtr[i] == ' ') lastSpaceIdx = i;
      charsInLine++;
    }

    // If string is longer than maxChars, wrap at the last space
    int wrapPoint = (charsInLine < maxChars || lastSpaceIdx == -1) ? charsInLine : lastSpaceIdx;
    
    // Copy the line to a temporary buffer for centering calculation
    strncpy(lineBuffer, ptr, wrapPoint);
    lineBuffer[wrapPoint] = '\0';

    // Calculate X to center the text
    int textWidth = strlen(lineBuffer) * (6 * fontSize);
    int x = (SCREEN_WIDTH - textWidth) / 2;

    oled.setCursor(x, y);
    oled.print(lineBuffer);

    // Move pointers and Y coordinate for next line
    ptr += wrapPoint;
    if (*ptr == ' ') ptr++; // Skip the space we wrapped on
    y += lineHeight;
    
    if (y > SCREEN_HEIGHT - lineHeight) break; // Don't draw off-screen
  }
}

/// QUOTES

// Define the actual text strings in Flash
const char q0[] PROGMEM = "Calculating quantum fluctuations";
const char q1[] PROGMEM = "Asking the spirits";
const char q2[] PROGMEM = "Checking the constellations";
const char q3[] PROGMEM = "Wondering";
const char q4[] PROGMEM = "Consulting the Higher Ones";
const char q5[] PROGMEM = "Pondering the orb";
const char q6[] PROGMEM = "Running the numbers";
const char q7[] PROGMEM = "Uhhhh";
const char q8[] PROGMEM = "Hmmmm";
const char q9[] PROGMEM = "Interesting";
const char q10[] PROGMEM = "Making some coffee";
const char q11[] PROGMEM = "Drawing a magic square";
const char q12[] PROGMEM = "Reading your palm";

const char f0[] PROGMEM = "Your days will be filled with sunshine!";
const char f1[] PROGMEM = "A difference must make a difference.";
const char f2[] PROGMEM = "The wheel of fortune is turning your way!";
const char f3[] PROGMEM = "Luck will visit you on the next new moon!";
const char f4[] PROGMEM = "Your wisdom will find a way!";
const char f5[] PROGMEM = "The star of riches is shining upon you!";
const char f6[] PROGMEM = "Cheese";
const char f7[] PROGMEM = "Your ability to find the silly in the serious will take you far.";
const char f8[] PROGMEM = "You will soon be honored by someone you respect.";
const char f9[] PROGMEM = "You will cotinue to interpret vague statements as uniquely meaningful.";
const char f10[] PROGMEM = "Error 404: fortune not found!";
const char f11[] PROGMEM = "Live, laugh, love or something...";
const char f12[] PROGMEM = "Before you can see the light, you have to deal with the darkness.";
const char f13[] PROGMEM = "Never give up, never let down, never turn around and desert someone";
const char f14[] PROGMEM = "What's that in your eye? Oh... it's a sparkle!";
const char f15[] PROGMEM = "Don't have more than 2 drinks, or you'll have a hangover tomorrow";
 
const char no_hand_quote[] PROGMEM = "Listen, dearie. Place your hand under the eye and let the spirits reveal the wondrous things your future has in store!";

// Create the tables (arrays of pointers) also in Flash
const char* const generating_quotes[] PROGMEM = { q0, q1, q2, q3, q4, q5, q6, q7, q8, q9, q10, q11, q12 };
const char* const fortune_quotes[] PROGMEM = { f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15 };

const unsigned int NO_GEN_QUOTES = 13;
const unsigned int NO_FORTUNES = 16;

short last_gen_quote = -1;
short current_gen_quote = -1;
short current_fortune = -1;

int quote_timer = QUOTE_DISPLAY_TIME;

void get_random_generating_quote_index() {
  short new_quote;
  do {
    new_quote = random(0, NO_GEN_QUOTES);
  } while (new_quote == last_gen_quote && NO_GEN_QUOTES > 1); 
  
  current_gen_quote = new_quote;
  last_gen_quote = new_quote;
}

void get_random_fortune_index() {
  current_fortune = random(0, NO_FORTUNES);
}

/// MUSIC

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

const int melody[] PROGMEM = {
  REST, 4, NOTE_G5, 4,
  NOTE_A5, 4, NOTE_AS5, 4,
  NOTE_A5, 4, NOTE_F5, 4,
  NOTE_A5, 4, NOTE_G5, 4,
  REST, 4, NOTE_G5, 4,
  NOTE_A5, 4, NOTE_AS5, 4,
  NOTE_C6, 4, NOTE_AS5, 4,

  NOTE_A5, 4, NOTE_G5, 4, //8
  REST, 4, NOTE_G5, 4,
  NOTE_A5, 4, NOTE_AS5, 4,
  NOTE_A5, 4, NOTE_F5, 4,
  NOTE_A5, 4, NOTE_G5, 4,
  NOTE_D6, 4, REST, 8, NOTE_C6, 8,
  REST, 4, NOTE_AS5, 4,

  NOTE_A5, 4, NOTE_AS5, 8, NOTE_C6, 8, //15
  NOTE_F6, 8, REST, 8, REST, 4,
  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,

  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16, //20
  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,

  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, //25
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_AS5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,

  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_C6, 16, NOTE_C6, 16, NOTE_F6, 16, NOTE_D6, 8, REST, 16, REST, 8,
  REST, 4, NOTE_C6, 16, NOTE_AS5, 16,

  NOTE_C6, -8,  NOTE_F6, -8, NOTE_D6, -4, //35
  NOTE_C6, 8, NOTE_AS5, 8,
  NOTE_C6, 8, NOTE_F6, 16, NOTE_D6, 8, REST, 16, REST, 8,
  REST, 4, NOTE_C6, 8, NOTE_D6, 8,
  NOTE_DS6, -8, NOTE_F6, -8,
};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

bool play_music = false;
unsigned int music_counter = 0;
int music_timer = 0;

void music_thread() {
  if (!play_music) return;
  if (music_timer > 0) {
    music_timer -= DELTA_T;
    return;
  }

  unsigned int note_duration_ms = 0;
  int note_duration = pgm_read_word(&melody[music_counter +1]);
  
  if (note_duration < 0) {
    note_duration_ms = (wholenote / abs(note_duration)) * 1.5;
  } else {
    note_duration_ms = wholenote / note_duration;
  }

  music_timer = note_duration_ms;
  tone(BUZZER_PIN, pgm_read_word(&melody[music_counter]), note_duration_ms*0.9);
  
  music_counter += 2;
  if (music_counter >= (notes*2)-1) music_counter = 0;
}

void start_music() {
  play_music = true;
}

void pause_music() {
  play_music = false;
}

void stop_music() {
  play_music = false;
  music_timer = 0;
  music_counter = 0;
}

/// ULTRASONIC

float get_ultrasonic_distance() {
  digitalWrite(TRIGGERPIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIGGERPIN, HIGH);
  delayMicroseconds(50);
  digitalWrite(TRIGGERPIN, LOW);

  float duration = pulseIn(ECHOPIN, HIGH);
  return duration * 0.0344 / 2;
}

bool is_hand_inserted(float distance) {
  return (distance < HAND_DISTANCE_THRESHOLD);
}

/// MAIN

void setup() {
  oled.begin(SSD1306_SWITCHCAPVCC);

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE); // otherwise text is not visible
  
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGGERPIN, OUTPUT);

  randomSeed(analogRead(0)); // give random seed to the pseudorandom generator
}

short dot_count = 0;
short round_count = 0;

void loop() {
  update_delta_t();
  float detected_distance = get_ultrasonic_distance();
  oled.clearDisplay();

  // 80 bytes is enough for your longest fortune without crashing the Uno
  char messageBuffer[128]; 
  memset(messageBuffer, 0, sizeof(messageBuffer));
  
  if (is_hand_inserted(detected_distance) && hand_time < HAND_TIME_BUFFER) {
    hand_time++;
  } else if (hand_time > 0) {
    hand_time--;
  }

  if (hand_time > 0) {
    start_music();
    
    if (quote_timer > 0) {
      if (current_gen_quote == -1) get_random_generating_quote_index();
      
      strcpy_P(messageBuffer, (char*)pgm_read_word(&generating_quotes[current_gen_quote]));

      if (++dot_count > 3) dot_count = 0;
      int len = strlen(messageBuffer);
      for(int i = 0; i < dot_count && (len + i < 79); i++) {
        messageBuffer[len + i] = '.';
      }
      messageBuffer[len + dot_count] = '\0';
        
      quote_timer -= DELTA_T;
      
    } else if (round_count < NO_ROUNDS) {
      round_count++;
      quote_timer = QUOTE_DISPLAY_TIME;
      current_gen_quote = -1;
      
    } else {
      if (current_fortune == -1) get_random_fortune_index();
      
      strcpy_P(messageBuffer, (char*)pgm_read_word(&fortune_quotes[current_fortune]));
    }
    
  } else {
    stop_music();

    strcpy_P(messageBuffer, (char*)no_hand_quote);

    round_count = 0;
    current_gen_quote = -1;
    current_fortune = -1;
    quote_timer = QUOTE_DISPLAY_TIME;
  }

  centeredWordWrap(messageBuffer);
  oled.display();

  delay(100);
  music_thread();
}
