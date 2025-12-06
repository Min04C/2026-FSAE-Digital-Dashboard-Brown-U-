// 2026 shifting lights

// --------------------------------------------------------------------
//This code updates each LED sequentually


#include <WS2812Serial.h>

// Uses pin 8 on Teensy with Serial2 communication

const int numled = 8;
const int pin = 8;

byte drawingMemory[numled*3];         //  3 bytes per LED for RGB
DMAMEM byte displayMemory[numled*12]; // 12 

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_RGB);

//              BLUE          | GREEN      | RED  
#define RED     ((0 <<  16)   | (0 << 8)   | 255)
#define GREEN   ((0 << 16)   | (255 << 8) | 0)
#define BLUE    ((255 << 16)   | (0 << 8)   | 0)
#define YELLOW  ((0 << 16) | (255 << 8) | 255)
#define PINK    ((255 << 16) | (0 << 8)   | 255)
#define ORANGE  ((0 << 16) | (128 << 8) | 255)
#define WHITE   ((255 << 16) | (255 << 8) | 255)
#define OFF     0  // for loop 2

// for loop 2
/*uint32_t rpmStates[15][16] = {
  // Range 0 
  {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 1
  {OFF, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 2
  {OFF, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 3
  {OFF, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 4
  {OFF, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 5
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 6
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 7
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 8 (first yellow LED)
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 9
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 10
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, YELLOW, OFF, OFF, OFF, OFF, OFF},
  // Range 11 (first red LED)
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, YELLOW, RED, OFF, OFF, OFF, OFF},
  // Range 12
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, YELLOW, RED, RED, OFF, OFF, OFF},
  // Range 13
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, YELLOW, RED, RED, RED, OFF, OFF},
  // Range 14 (all LEDs filled except first and last)
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, YELLOW, RED, RED, RED, RED, OFF},
  //{OFF, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, OFF}
};*/
uint32_t rpmStates[15][16] = {
  // Range 0
  {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 1
  {OFF, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 2
  {OFF, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 3
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 4
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 5
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 6
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, OFF, OFF, OFF, OFF, OFF, OFF},
  // Range 7
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, OFF, OFF, OFF, OFF},
  // Range 8 (first yellow LED)
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, OFF, OFF, OFF},
  // Range 9
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, YELLOW, YELLOW, YELLOW, YELLOW, OFF},
  // Range 10
  {OFF, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, ORANGE, ORANGE, ORANGE, ORANGE, OFF},
  // Range 11 (first red LED)
  {OFF, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, OFF},
  // Range 12
  {OFF, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, OFF},
  // Range 13
  {OFF, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, OFF},
  // Range 14 (all LEDs filled except first and last)
  {OFF, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, OFF},
  //{OFF, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, OFF}
};

uint32_t ledState[16];
bool LEDs_on = true;
bool isRed = false;
bool wasRed = false;


void setup() {
  leds.begin();
  leds.setBrightness(35); // 0=off, 255=brightest
}

void loop() {
  
  
  
  // 1. change all the LEDs in 1.5 seconds (NO RPM)
  /*int microsec = 1500000 / leds.numPixels();
  colorWipe(RED, microsec);
  colorWipe(GREEN, microsec);
  colorWipe(BLUE, microsec);
  colorWipe(YELLOW, microsec);
  colorWipe(PINK, microsec);
  colorWipe(ORANGE, microsec);
  colorWipe(WHITE, microsec);*/

  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, RED);     // update library's internal memory for led i
    leds.show();                 // print to led i
    delayMicroseconds(wait_us);  //
  }
  
  /*static int rpm = 0;
  static int step = 1;
  //colorGYR(rpm); // 2. RPM INPUT - not smooth, deterministic 0-7g, 8-10y, 11-13.5r
  colorBrightnessBlend(rpm); // 2. RPM INPUT - brightness blend, deterministic 0-7g, 8-10y, 11-13.5r
  leds.show();

  static unsigned long lastFlash = 0;
  if (isRed) {
    if (millis() - lastFlash > 100) {
      toggleFlash();
      lastFlash = millis();
    }
  } else if (wasRed) {
    LEDs_on = true;
    for (int i = 0; i < numled; i++) {
      leds.setPixel(i, ledState[i]);
    }
    leds.show();
  }

  wasRed = isRed;

  rpm += step;
  if (rpm >= 13500 || rpm <= 0) step = -step;*/


}

void colorWipe(int color, int wait_us) {
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);     // update library's internal memory for led i
    leds.show();                 // print to led i
    delayMicroseconds(wait_us);  //
  }
}

void colorGYR(int rpm) {
  int tr = rpm;
  if (rpm < 0) {tr = 0;} 
  else if (rpm > 13500) {tr = 13500;}
  int range = (tr * 14) / 13500; // map 0-13500 to 0-14
  for (int i = 1; i < (numled - 1); i++) {
    leds.setPixel(i, rpmStates[range][i]);
  }
  leds.show();
}

/*void colorBrightnessBlend(int rpm) {
  int tr = rpm;
  if (rpm < 0) {tr = 0;} 
  else if (rpm > 13500) {tr = 13500;}

  for (int i = 1; i < (numled - 1); i++) {
    int min = (i - 1) * 1000;
    int max = (i < 14) ? (i * 1000) : (13500);

    uint8_t brightness;
    if (tr < min) {brightness = 0;}
    else if (tr >= max) {brightness = 255;} 
    else {brightness = (rpm - min)} 


  }

  if (range)
}*/

void colorBrightnessBlend(int rpm) {
  int tr = rpm;
  if (rpm < 0) {tr = 0;} 
  else if (rpm > 13500) {tr = 13500;}

  int full = tr / 1000; // map 0-13500 to 0-14
  for (int i = 1; i < (numled - 1); i++) {       // FILL IN FULL LEDS (1000s)
    leds.setPixel(i, rpmStates[full][i]);
    ledState[i] = rpmStates[full][i];
  }
  
  // FILL IN HALF LEDS (100s)
  int trfrac = tr % 1000;
  if (trfrac > 0) {
    int ledbrit = (full < 14) ? (trfrac * 255 / 1000) : (trfrac * 255 / 500);
    uint32_t color;
    if (full < 8) { // green
      color = (0 << 16) | (ledbrit << 8) | 0;
      isRed = false;
    } else if (full < 11) { // yellow
      color = (ledbrit << 16) | (ledbrit << 8) | 0;
      isRed = false;
    } else { // red
      color = (ledbrit << 16) | (0 << 8) | 0;
      isRed = true;
    }
    leds.setPixel((full + 1), color);
    ledState[full + 1] = color;
  
  }                          
  
}

void toggleFlash() {
  LEDs_on = !LEDs_on;
  for (int i = 0; i < 16; i += 1) {
    if (LEDs_on) {
      leds.setPixel(i, ledState[i]);
    } else {
      leds.setPixel(i, 0);
    }
  }
  leds.show();
}


