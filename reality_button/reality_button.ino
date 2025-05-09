#include <FastLED.h>
#include <VibrationMotor.h>

#define NUM_LEDS 12
#define DATA_PIN 7
#define BUTTON_PIN 12
#define MINCYCLE 500
#define CHARGESTEPS 10
#define BUZZERPIN 8
#define VIBMOTORPIN 5

enum state : byte {IDLE, CHARGING, COOLDOWN};
int idlecounter = 0;

void cycle(void);

CRGB leds[NUM_LEDS];
unsigned long LastCycle = 0;

state cyclestate = IDLE;

VibrationMotor vibe(VIBMOTORPIN);

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZERPIN, OUTPUT);
}

void idlecycle() {

  blackout();
  idlecounter = idlecounter % NUM_LEDS;

  leds[idlecounter] = CRGB::Blue;
  leds[(idlecounter -1) % NUM_LEDS].setRGB( 0, 0, 64);
  leds[(idlecounter + 1) % NUM_LEDS].setRGB( 0, 0, 64);

  idlecounter++;

  FastLED.show();
}

void blackout() {

  // Black out all the lights
  for (int i=0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;  
  }
  FastLED.show();
  vibe.off();

  
}

void charging_error() {
  int duration = 5000;
  unsigned long starttime;
  
  starttime = millis();
  blackout();

  while ( millis() - starttime < duration) {
    fill_solid( leds, NUM_LEDS, CRGB( 255, 0, 0) );
    FastLED.show();
    tone(BUZZERPIN, 600, 500);
    delay(500);
    noTone(BUZZERPIN);
    blackout();
    delay(500);
  }
  cooldown();
}

void chargecycle() {
  byte buttonState;
  int stepdelay = 10;
  unsigned long offset;
  unsigned long effecttime;
  int length = 4000;
  int brightness;
  int prev_brightness;
  int rate;
  blackout();

  offset = millis();

  effecttime = 0;
  brightness = 0;
  while ( effecttime < length) {
    effecttime = millis() - offset;
    if ( effecttime > 500) {
      buttonState = digitalRead(BUTTON_PIN);
      if (buttonState == LOW ) {
        // They broke it!  Jump to error
        charging_error();
        return;
      }
    }
    rate = 60 + (effecttime/1000 * 60);
    prev_brightness = brightness;
    brightness =  round((beatsin8(rate, 0, 255, 0, 128) * 0.9) + (prev_brightness * 0.1));
    fill_solid( leds, NUM_LEDS, CRGB( brightness, brightness, 0) );
    FastLED.show();
    if ( brightness > 200 && prev_brightness <= 200) {
      tone(BUZZERPIN, 440, 200);
      vibe.on(((float)effecttime / length) * 256);
    }
    if ( brightness < 200 && prev_brightness >= 200) {
      noTone(BUZZERPIN);
    }
    delay(stepdelay);
  }
  noTone(BUZZERPIN);
  vibe.off();
  explode();
}

void explode() {
  unsigned long starttime;
  int length = 5000;
  int brightness;
  fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255) );
  FastLED.show();
  delay(1000);
  starttime = millis();
  while ( millis() - starttime < length ) {
    brightness = floor( (1 - ((millis() - starttime) / (float)length)) * 200 );
    fill_solid( leds, NUM_LEDS, CRGB( brightness, brightness, brightness));
    if ( random(0,100) < 15 ) {
      //int sparkle = floor( ((millis() - starttime) / (float)length) * 255 );
      int sparkle = 255;
      //leds[random(0, NUM_LEDS)] = CRGB( sparkle, sparkle, sparkle);
      leds[random(0, NUM_LEDS)] = CRGB( random(brightness,255), random(brightness,255), random(brightness,255));
    }
    FastLED.show();
    delay(10);
  }
  cooldown();
}

void cooldown() {
  unsigned long starttime;
  int length = 10000;

  starttime = millis();
  blackout();
  delay(2000);
  while ( millis() - starttime < length) {
    fill_solid( leds, NUM_LEDS, CRGB( beatsin8(20, 0, 16), 0, 0));
    FastLED.show();
    
    delay(10);
  }
  fill_solid( leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(1000);
  tone(BUZZERPIN, 659, 50);
  delay(50);
  tone(BUZZERPIN, 880, 50);
  blackout();
}

void cycle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB( random(0,255), random(0,255), random(0,255));
  }
  
  FastLED.show();

}

void loop() {
  byte buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == HIGH) {
      idlecycle();
  } else {
    // Explode!
    chargecycle();
  }

    delay(100);
}
