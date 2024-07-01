#include <FastLED.h>
#include "FastLED_RGBW.h"
#include <CAN.h>

#define LED_PIN 5
#define LED_PIN2 9
#define NUM_LEDS 63
#define PIN_CS_CAN 7
#define POZITIE_PIN 4
#define SEMNALIZARE_PIN 2
#define FRANA_PIN 3
#define SEMN_STG 6
#define SEMN_DRT 8

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];
CRGBW leds2[NUM_LEDS];
CRGB *leds2RGB = (CRGB *) &leds2[0];
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
struct semnalizator{
  int onoff;
  int intensitate;
  int delay;
  bool animatie;
};
semnalizator Semnalizare;
struct pozitie{
  int onoff;
  int stil;
  int intensitate;
  bool animatie;
  int delay;
};
pozitie Pozitie;
int franaOnOff;
int pozitieIntensitateBackup;
int showmode;
int pozitieStilBackup = 0;

int pozS[3][11];
int pozJ[3][10];

int intensitatePozitie;
bool animatiePozitie;
bool starePozitie;
int intensitateSemnalizare;
int delaySemnalizare;
bool animatieSemnalizare;
bool stareSemnalizare;
int stilSetari;
bool showmodeSetari;
bool showmodeIntrerupator;

unsigned long previousMillis = 0UL;
int semnalizareStateSimpla1 = 0;
int semnalizareStateSimpla2 = 0;
int semnalizareStateSecventiala1 = 0;
int semnalizareStateSecventiala2 = 0;
int pozitieState1=0;
int pozitieState2=0;

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
  FastLED.addLeds<WS2812B, LED_PIN2, RGB>(leds2RGB, getRGBWsize(NUM_LEDS));
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);
  // FastLED.setBrightness(20);
  FastLED.clear();
  FastLED.show();
  Serial.begin(9600);

  while (!Serial);
  Serial.println("CAN Receiver");
  CAN.setPins(PIN_CS_CAN);
  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  initializare();
  pinMode(POZITIE_PIN, INPUT_PULLUP);
  pinMode(SEMNALIZARE_PIN, INPUT_PULLUP);
  pinMode(FRANA_PIN, INPUT_PULLUP);
  pinMode(SEMN_DRT, INPUT_PULLUP);
  pinMode(SEMN_STG, INPUT_PULLUP);
  
  Semnalizare.onoff=0;
  Semnalizare.animatie=false;
  Semnalizare.delay=100;
  Semnalizare.intensitate=20;

  Pozitie.onoff=0;
  Pozitie.animatie=false;
  Pozitie.delay=50;
  Pozitie.stil=1;
  Pozitie.intensitate=20;
  pozitieIntensitateBackup = Pozitie.intensitate;
  franaOnOff=0;
  showmode=0;
}

void loop() {
  receiveDataCAN();

  if(Pozitie.onoff == 0){// && Semnalizare.onoff == 0){
    FastLED.clear(); // Stinge toate LED-urile
    FastLED.show();
    Pozitie.stil = stilSetari;
    showmode = showmodeSetari;
  }
  if(showmode==1 && showmodeIntrerupator == 1 && Pozitie.onoff==0){
    // rainbowLoop();
    rainbow();
  }
  else{
    showPozitie();
    showSemnalizare();
  }
  int poz = digitalRead(POZITIE_PIN);
  int semn = digitalRead(SEMNALIZARE_PIN);
  int fran = digitalRead(FRANA_PIN);
  if(semn==0){
    Semnalizare.onoff = 1;
  }
  else{
    Semnalizare.onoff = 0;
    clearSemnalizare();
  }
  if(poz==0){
    Pozitie.onoff = 1;
  }
  else{
    Pozitie.onoff = 0;
  }
  if(fran==0){
    Pozitie.onoff = 1;
    Pozitie.intensitate=100;
    Pozitie.animatie = 0;
  }
  else{
    Pozitie.intensitate = intensitatePozitie;
    Pozitie.animatie = animatiePozitie;
  }
}

void receiveDataCAN(){
  int packetSize = CAN.parsePacket();
  
  if (packetSize) {
    if (CAN.packetId() == 0x123) {
      uint8_t data[8];
      // Citește datele din primul pachet
      for (int i = 0; i < 8; i++) {
        data[i] = CAN.read();
      }
      // Reconstruiește variabilele din primul pachet
      intensitatePozitie = data[0];
      animatiePozitie = data[1] == 1 ? true : false;
      intensitateSemnalizare = data[2];
      delaySemnalizare = data[3];
      animatieSemnalizare = data[4] == 1 ? true : false;
      stilSetari = data[5];
      showmodeSetari = data[6] == 1 ? true : false;
      showmodeIntrerupator = data[7] == 1 ? true : false;
    } 
    Semnalizare.intensitate = intensitateSemnalizare;
    Semnalizare.delay = delaySemnalizare;
    Semnalizare.animatie = animatieSemnalizare;
  }
}

void showPozitie(){
  CRGB colorRed = CRGB((Pozitie.intensitate*255)/100, 0, 0);
  if(Pozitie.onoff == 0){
    pozitieStart=0;
    if(Pozitie.stil == 1){
      pozitieLoop1(CRGB::Black,0);
    }
    else if(Pozitie.stil == 2){
      pozitieLoop2(CRGB::Black,0);
    }
  }
  else if(Pozitie.onoff == 1){
    if(Pozitie.stil == 1){
      if(Pozitie.animatie == true){
        pozitieLoop1(colorRed,Pozitie.delay);
      }
      else if(Pozitie.animatie == false){
        pozitieLoop1(colorRed,0);
      }
    }
    else if(Pozitie.stil == 2){
      if(Pozitie.animatie == true){
        pozitieLoop2(colorRed,Pozitie.delay);
      }
      else if(Pozitie.animatie == false){
        pozitieLoop2(colorRed,0);
      }
    }
  } 
}

void showSemnalizare(){
  CRGB color = CRGB((Semnalizare.intensitate*255)/100, (Semnalizare.intensitate*50)/100, 0);
  int procent = 100 - Semnalizare.delay;
  int delay_time_secvential = 80 * (1 + procent/100);
  int delay_time_simple =  400 * (1 + procent/100);
  if(Semnalizare.onoff == 1){
    if(Pozitie.stil == 1){
      if(Semnalizare.animatie == true){
        semnalizareSecventiala1(color, delay_time_secvential);
      }
      else if(Semnalizare.animatie == false){
        semnalizareSimpla1(color, delay_time_simple);
      }
    }
    else if(Pozitie.stil == 2){
      if(Semnalizare.animatie == true){
        semnalizareSecventiala2(color, delay_time_secvential);
      }
      else if(Semnalizare.animatie == false){
        semnalizareSimpla2(color, delay_time_simple);
      }
    }
  }
}

void clearSemnalizare(){
  if(Pozitie.stil==1){
    colorFill(CRGB::Black, startS, endS);
  }
  else if(Pozitie.stil==2){
    for(int i=2*colsS;i<=endS-3;i++){
      leds[i] = CRGB::Black;
    }
    for(int i=startJ;i<=startJ+colsJ-3;i++){
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
}

void pozitieLoop1(CRGB c, int delay_time){
  if(Pozitie.onoff==1 && pozitieStart==0){ //folosita pentru o singura animatie de pornire
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= delay_time) {
      previousMillis = currentMillis;
      switch(pozitieState1){
        case 0 : leds[startJ] = c; FastLED.show(); pozitieState1 = 1; break;
        case 1 : leds[startJ+1] = c; FastLED.show(); pozitieState1 = 2; break;
        case 2 : leds[startJ+2] = c; FastLED.show(); pozitieState1 = 3; break;
        case 3 : leds[startJ+2] = c; FastLED.show(); pozitieState1 = 4; break;
        case 4 : leds[startJ+2] = c; FastLED.show(); pozitieState1 = 5; break;
        case 4 : leds[startJ+2] = c; FastLED.show(); pozitieState1 = 6; break;
        case 5 : leds[startJ+2] = c; FastLED.show(); pozitieState1 = 7; break;
      }
    }
    
    for(int i = startJ; i < endJ; i++){
        leds[i] = c;
        if(delay_time!=0){
          FastLED.show();
          delay(delay_time);
        }
    }
    if(delay_time==0){
      FastLED.show();
    }
    pozitieStart=1;
  }
  else if(Pozitie.onoff==1){
    for(int i = startJ; i < endJ; i++){
        leds[i] = c;
    }
    FastLED.show();
  }
}

void pozitieLoop2(CRGB c, int delay_time){
  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= delay_time) {
  //   previousMillis = currentMillis;
  //   switch(pozitieState2){
  //     case 0 : colorFill(c, startS, endS); semnalizareStateSimpla1 = 1; break;
  //     case 1 : colorFill(CRGB::Black, startS, endS); semnalizareStateSimpla1 = 0; break;
  //   }
  // }


  if(Pozitie.onoff==1 && pozitieStart==0){ //folosita pentru o singura animatie de pornire
    for(int i = 0; i < colsS; i++){
        leds[i] = c;
        leds[2*colsS - i-1] = c;
        if(delay_time!=0){
          FastLED.show();
          delay(delay_time);
        }
    }
    leds[colsS*3-1] = c;
    leds[colsS*3-2] = c;
    if(delay_time!=0){
      FastLED.show();
      delay(delay_time);
    }
    leds[startJ+9] = c;
    leds[startJ+10] = c;
    if(delay_time!=0){
      FastLED.show();
      delay(delay_time);
    }
    for(int i = colsJ-1 ; i >= 0; i--){
        leds[51 - i] = c;
        leds[52 + i] = c;
        if(delay_time!=0){
          FastLED.show();
          delay(delay_time);
        }
    }
    if(delay_time==0){
      FastLED.show();
    }
    pozitieStart=1;
  }
  else if(Pozitie.onoff==1){
    for(int i = 0; i < colsS; i++){
        leds[i] = c;
        leds[2*colsS - i-1] = c;
    }
    leds[colsS*3-1] = c;
    leds[colsS*3-2] = c;
    leds[startJ+9] = c;
    leds[startJ+10] = c;
    for(int i = colsJ-1 ; i >= 0; i--){
        leds[51 - i] = c;
        leds[52 + i] = c;
    }
    FastLED.show();
  }
}

void semnalizareSimpla1(CRGB c, int delay_time){
  // colorFill(c, startS, endS);
  // delay(delay_time);
  // colorFill(CRGB::Black, startS, endS);
  // delay(delay_time);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay_time) {
    previousMillis = currentMillis;
    switch(semnalizareStateSimpla1){
      case 0 : colorFill(c, startS, endS); semnalizareStateSimpla1 = 1; break;
      case 1 : colorFill(CRGB::Black, startS, endS); semnalizareStateSimpla1 = 0; break;
    }
  }
}

void semnalizareSimpla2(CRGB c, int delay_time){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay_time) {
    previousMillis = currentMillis;
    switch(semnalizareStateSimpla2){
      case 0 : {
        for(int i=2*colsS;i<=endS-3;i++){
          leds[i] = c;
        }
        for(int i=startJ;i<=startJ+colsJ-3;i++){
          leds[i] = c;
        }
        FastLED.show();
        semnalizareStateSimpla2 = 1; 
        break;
      }
      case 1 : {
        for(int i=2*colsS;i<=endS-3;i++){
          leds[i] = CRGB::Black;
        }
        for(int i=startJ;i<=startJ+colsJ-3;i++){
          leds[i] = CRGB::Black;
        }
        FastLED.show(); 
        semnalizareStateSimpla2 = 0; 
        break;
      }
    }
  }

  // for(int i=2*colsS;i<=endS-3;i++){
  //   leds[i] = c;
  // }
  // for(int i=startJ;i<=startJ+colsJ-3;i++){
  //   leds[i] = c;
  // }
  // FastLED.show();
  // delay(delay_time);
  // for(int i=2*colsS;i<=endS-3;i++){
  //   leds[i] = CRGB::Black;
  // }
  // for(int i=startJ;i<=startJ+colsJ-3;i++){
  //   leds[i] = CRGB::Black;
  // }
  // FastLED.show();
  // delay(delay_time);
}

void semnalizareSecventiala1(CRGB c, int delay_time) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay_time) {
    previousMillis = currentMillis;
    switch(semnalizareStateSecventiala1){
      case 0: functieSemnalizareSecventiala1(c,0); semnalizareStateSecventiala1 = 1; break;
      case 1: functieSemnalizareSecventiala1(c,3); semnalizareStateSecventiala1 = 2; break;
      case 2: functieSemnalizareSecventiala1(c,6); semnalizareStateSecventiala1 = 3; break;
      case 3: functieSemnalizareSecventiala1(c,9); semnalizareStateSecventiala1 = 4; break;
      case 4: functieSemnalizareSecventiala1(CRGB::Black,0); semnalizareStateSecventiala1 = 5; break;
      case 5: functieSemnalizareSecventiala1(CRGB::Black,3); semnalizareStateSecventiala1 = 6; break;
      case 6: functieSemnalizareSecventiala1(CRGB::Black,6); semnalizareStateSecventiala1 = 7; break;
      case 7: functieSemnalizareSecventiala1(CRGB::Black,9); semnalizareStateSecventiala1 = 0; break;
    }
  }
  // for (int i = 0; i < colsS; i=i+3) {
  //   leds[pozS[0][i]] = c;
  //   leds[pozS[1][i]] = c;
  //   leds[pozS[2][i]] = c;
  //   if(i+1<colsS){
  //     leds[pozS[0][i+1]] = c;
  //     leds[pozS[1][i+1]] = c;
  //     leds[pozS[2][i+1]] = c;
  //   }
  //   if(i+2<colsS){
  //     leds[pozS[0][i+2]] = c;
  //     leds[pozS[1][i+2]] = c;
  //     leds[pozS[2][i+2]] = c;
  //   }
  //   FastLED.show();
  //   delay(delay_time);
  // }
  // for (int i = 0; i < colsS; i=i+3) {
  //   leds[pozS[0][i]] = CRGB::Black;
  //   leds[pozS[1][i]] = CRGB::Black;
  //   leds[pozS[2][i]] = CRGB::Black;
  //   if(i+1<colsS){
  //     leds[pozS[0][i+1]] = CRGB::Black;
  //     leds[pozS[1][i+1]] = CRGB::Black;
  //     leds[pozS[2][i+1]] = CRGB::Black;
  //   }
  //   if(i+2<colsS){
  //     leds[pozS[0][i+2]] = CRGB::Black;
  //     leds[pozS[1][i+2]] = CRGB::Black;
  //     leds[pozS[2][i+2]] = CRGB::Black;
  //   }
  //   FastLED.show();
  //   delay(delay_time);
  // }
}

void functieSemnalizareSecventiala1(CRGB c, int i){
  leds[pozS[0][i]] = c;
  leds[pozS[1][i]] = c;
  leds[pozS[2][i]] = c;
  if(i+1<colsS){
    leds[pozS[0][i+1]] = c;
    leds[pozS[1][i+1]] = c;
    leds[pozS[2][i+1]] = c;
  }
  if(i+2<colsS){
    leds[pozS[0][i+2]] = c;
    leds[pozS[1][i+2]] = c;
    leds[pozS[2][i+2]] = c;
  }
  FastLED.show();
}

void functieSemnalizareSecventiala2(CRGB c, int i, int j, int impar = 0){
  if(impar==1){
    leds[i] = c; leds[i-1] = c;
  }
  else{
    leds[i] = c; leds[i-1] = c; leds[i-2] = c;
  }
  leds[j] = c; leds[j-1] = c; leds[j-2] = c;
  FastLED.show();
}

void semnalizareSecventiala2(CRGB c, int delay_time){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay_time) {
    previousMillis = currentMillis;
    switch(semnalizareStateSecventiala2){
      case 0: functieSemnalizareSecventiala2(c, 2*colsS+7, startJ+8); semnalizareStateSecventiala2 = 1; break;
      case 1: functieSemnalizareSecventiala2(c, 2*colsS+7-3, startJ+8-3); semnalizareStateSecventiala2 = 2; break;
      case 2: functieSemnalizareSecventiala2(c, 2*colsS+7-6, startJ+8-6, 1); semnalizareStateSecventiala2 = 3; break;
      case 3: functieSemnalizareSecventiala2(CRGB::Black, 2*colsS+7, startJ+8); semnalizareStateSecventiala2 = 4; break;
      case 4: functieSemnalizareSecventiala2(CRGB::Black, 2*colsS+7-3, startJ+8-3); semnalizareStateSecventiala2 = 5; break;
      case 5: functieSemnalizareSecventiala2(CRGB::Black, 2*colsS+7-6, startJ+8-6,1); semnalizareStateSecventiala2 = 0; break;
    }
  }

  // int i = 2*colsS+7, j = startJ+8, x = 2*colsS+7, y = startJ+8;
  // leds[i] = c; leds[i-1] = c; leds[i-2] = c;
  // leds[j] = c; leds[j-1] = c; leds[j-2] = c;
  // i = i - 3; j = j - 3;
  // FastLED.show();
  // delay(delay_time);
  // leds[i] = c; leds[i-1] = c; leds[i-2] = c;
  // leds[j] = c; leds[j-1] = c; leds[j-2] = c;
  // i = i - 3; j = j - 3;
  // FastLED.show();
  // delay(delay_time);
  // leds[i] = c; leds[i-1] = c;
  // leds[j] = c; leds[j-1] = c; leds[j-2] = c;
  // FastLED.show();
  // delay(delay_time);

  // leds[x] = CRGB::Black; leds[x-1] = CRGB::Black; leds[x-2] = CRGB::Black;
  // leds[y] = CRGB::Black; leds[y-1] = CRGB::Black; leds[y-2] = CRGB::Black;
  // x = x - 3; y = y - 3;
  // FastLED.show();
  // delay(delay_time);
  // leds[x] = CRGB::Black; leds[x-1] = CRGB::Black; leds[x-2] = CRGB::Black;
  // leds[y] = CRGB::Black; leds[y-1] = CRGB::Black; leds[y-2] = CRGB::Black;
  // x = x - 3; y = y - 3;
  // FastLED.show();
  // delay(delay_time);
  // leds[x] = CRGB::Black; leds[x-1] = CRGB::Black;
  // leds[y] = CRGB::Black; leds[y-1] = CRGB::Black; leds[y-2] = CRGB::Black;
  // FastLED.show();
  // delay(delay_time);
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