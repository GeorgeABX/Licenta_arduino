#include <FastLED.h>
#include "FastLED_RGBW.h"
#define LED_PIN 5
#define NUM_LEDS 63

// CRGB leds[NUM_LEDS];

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];
int rows=3;
int colsS=10;
int colsJ=11;
int startS=0;
int endS=rows*colsS;
int startJ=endS;
int endJ=startJ+rows*colsJ;

bool pozitieOn = true;
bool pozitieStart = false;
bool semnalizareOn;
bool franaOn;
bool showmodeOn;

int read=0;
int semnalizator;
int pozitie;
int showmode;

int pozS[3][11];
int pozJ[3][10];

void initializare(){
  int i;
  for(i=colsS-1;i>=0;i--){
    pozS[0][colsS-i-1]=i;
  }
  for(i=colsS-1;i>=0;i--){
    pozS[1][i]=i+colsS;
  }
  for(i=colsS-1;i>=0;i--){
    pozS[2][colsS-i-1]=i+2*colsS;
  }
  for(i=colsJ-1;i>=0;i--){
    pozJ[0][colsJ-i-1]=i;
  }
  for(i=colsJ-1;i>=0;i--){
    pozJ[1][i]=i+colsJ;
  }
  for(i=colsJ-1;i>=0;i--){
    pozJ[2][colsJ-i-1]=i+2*colsJ;
  }
}

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);
  // FastLED.setBrightness(20);
  FastLED.clear();
  FastLED.show();
  Serial.begin(9600);
  initializare();
  semnalizator=0;
  pozitie=0;
  showmode=1;
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    read = Serial.read();
    semnalizator=read%10;
    pozitie=read/10%10;
    showmode=read/100%10;
  }
  
  // pozitieSimpla1();
  // pozitieLoop(CRGB(255,0,0),0);
  // delay(5000);
  // semnalizareSimpla1();
  // semnalizareSeventiala1(255, 50, 0, 80);
  if(showmode==1){
	  rainbowLoop();
  }
  // else{
  //   if(pozitie!=0){
  //     switch(pozitie){
  //       case 1:
  //         colorFill(CRGB::Red);
  //         break;
  //     }
  //   }
  // }

  FastLED.show();
}

void semnalizareSimpla1(){
  int red=255;
  int green=50;
  int blue=0;
  int delay_time=500;
  colorFill(CRGB(red, green, blue), startS, endS);
  delay(delay_time);
  colorFill(CRGB::Black, startS, endS);
  delay(delay_time);
}

void pozitieSimpla1(CRGB c){
  colorFill(c, startJ, endJ);
}
void pozitieLoop(CRGB c, int cnt){
  if(pozitieOn==1 && pozitieStart==0){
    delay(5000);
    for(int i = startJ; i < endJ; i++){
        leds[i] = c;
        FastLED.show();
        delay(50);
      }
    delay(5000);
    pozitieStart=1;
  }
  else if(pozitieOn==1 && pozitieStart==1){
    colorFill(c, startJ, endJ);
    // pozitieSimpla1();
  }
}

void semnalizareSecventiala1(uint8_t red, uint8_t green, uint8_t blue, int delay_time) {
   for (int i = 0; i < colsS; i=i+3) {
    // leds[i] = CRGB::Black; // Stinge toate LED-urile
    leds[pozS[0][i]] = CRGB(red, green, blue);
    leds[pozS[1][i]] = CRGB(red, green, blue);
    leds[pozS[2][i]] = CRGB(red, green, blue);
    if(i+1<colsS){
      leds[pozS[0][i+1]] = CRGB(red, green, blue);
      leds[pozS[1][i+1]] = CRGB(red, green, blue);
      leds[pozS[2][i+1]] = CRGB(red, green, blue);
    }
    if(i+2<colsS){
      leds[pozS[0][i+2]] = CRGB(red, green, blue);
      leds[pozS[1][i+2]] = CRGB(red, green, blue);
      leds[pozS[2][i+2]] = CRGB(red, green, blue);
    }
    FastLED.show();
    delay(delay_time);
  }
  for (int i = 0; i < colsS; i=i+3) {
    leds[pozS[0][i]] = CRGB::Black;
    leds[pozS[1][i]] = CRGB::Black;
    leds[pozS[2][i]] = CRGB::Black;
    if(i+1<colsS){
      leds[pozS[0][i+1]] = CRGB::Black;
      leds[pozS[1][i+1]] = CRGB::Black;
      leds[pozS[2][i+1]] = CRGB::Black;
    }
    if(i+2<colsS){
      leds[pozS[0][i+2]] = CRGB::Black;
      leds[pozS[1][i+2]] = CRGB::Black;
      leds[pozS[2][i+2]] = CRGB::Black;
    }
    // leds[i] = CRGB(red, green, blue); // Aprinde LED-ul curent
    FastLED.show();
    delay(delay_time);
  }
}







void colorFill(CRGB c, int start, int stop){
	for(int i = start; i < stop; i++){
		leds[i] = c;
	}
	FastLED.show();
}
void rainbow(){
	static uint8_t hue;
	for(int i = 0; i < NUM_LEDS; i++){
		leds[i] = CHSV((i * 256 / NUM_LEDS) + hue, 255, 255);
	}
	FastLED.show();
	hue++;
}
void rainbowLoop(){
	long millisIn = millis();
	long loopTime = 5000; // 5 seconds
	while(millis() < millisIn + loopTime){
		rainbow();
		delay(5);
	}
}
