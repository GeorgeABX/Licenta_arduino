#include <FastLED.h>
#include "FastLED_RGBW.h"
#define LED_PIN 4
#define NUM_LEDS 9

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

void color(){
  int red=255;
  int green=50;
  int blue=0;
  int delay_time=500;
  colorFill(CRGB(red, green, blue), 0, 9);
  delay(delay_time);
  // colorFill(CRGB::Black, startS, endS);
  // delay(delay_time);
}
void colorFill(CRGB c, int start, int stop){
	for(int i = start; i < stop; i++){
		leds[i] = c;
	}
	FastLED.show();
}
void fillWhite(){
	for(int i = 0; i < NUM_LEDS; i++){
		leds[i] = CRGBW(0, 0, 0, 255);
		FastLED.show();
	}
}
void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);
  FastLED.setBrightness(20);
  FastLED.clear();
  FastLED.show();
  

}

void loop() {
  // put your main code here, to run repeatedly:
  // colorFill(CRGB(255, 255, 255), 0, 9);
  // delay(1000);
  // fillWhite();
  // delay(1000);
  // colorFill(CRGB(255, 0, 0), 0, 9);
  // delay(1000);
  colorFill(CRGB(0, 0, 255), 0, 9);
  // delay(1000);
  // colorFill(CRGB(160,32,240), 0, 9);
  // delay(1000);
}
