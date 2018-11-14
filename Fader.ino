/*
Adafruit Arduino - Lesson 3. RGB LED
*/

int redPin = 9;
int greenPin = 10;
int bluePin = 11;

int delayMin = 50;
int delayMax = 500;

int photocellReading;
int photocellPin = 0;
int lightCutoff = 750;
//uncomment this line if using a Common Anode LED
#define COMMON_ANODE

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  

  Serial.begin(9600);
  randomSeed(analogRead(1));

  lcd.begin(16,2);
  lcd.print("Hello World!");
}


void loop()
{
  photocellReading = analogRead(photocellPin);
  lcd.setCursor(0,0);
  lcd.print("Photocell reading: ");
  lcd.setCursor(0,1);
  lcd.print(photocellReading);
 
  if ( photocellReading < lightCutoff ) {
    setColor(80, 0, 80);  // purple
    delay(int(random(delayMin, delayMax)));
    setColor(255, 70, 0); // orange
    delay(int(random(delayMin, delayMax)));    
  } else {
    setColor(0,0,0);
    delay(1000);
  }

}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
