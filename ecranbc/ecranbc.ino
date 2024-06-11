#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8

#define upPin 2
#define downPin 3
#define okPin 4
int firstStart = 1;
int selected = 0;
int currentMenu = 0; //0-main menu, 1-pozitie, 2-semnalizare, 3-frana, 4-setari

uint16_t screenWidth;
uint16_t screenHeight;
int partitieHeight;
int partitieWidth;

const char* modesMainMenu[] = {" Pozitie ", " Semnalizare ", " Frana ", " Setari "};
bool highlightedMainMenu[] = {false, false, false, false};
int highlightedCurrentMainMenu = 0;

const char* modesPozitieMenu[] = {" Intensitate ", " Animatie ", "Back"};
bool highlightedPozitieMenu[] = {false, false, false};
int highlightedCurrentPozitieMenu = 0;
int intensitatePozitie = 50;
bool animatiePozitie = false;

const char* modesSemnalizareMenu[] = {" Intensitate ", " Delay ", " Animatie ", " Viteza ", "Back"};
bool highlightedSemnalizareMenu[] = {false, false, false, false, false};
int highlightedCurrentSemnalizareMenu = 0;
int intensitateSemnalizare = 50;
int vitezaSemnalizare = 50;
int delaySemnalizare = 50;
bool animatieSemnalizare = false;

const char* modesFranaMenu[] = {" Emergency ", " Frecventa ", "Back"};
bool highlightedFranaMenu[] = {false, false, false};
int highlightedCurrentFranaMenu = 0;
bool blinkFrana = false;
int frecventaFrana = 50;

const char* modesSetariMenu[] = {" Stil ", " ShowMode ", "Back"};
bool highlightedSetariMenu[] = {false, false, false};
int highlightedCurrentSetariMenu = 0;
int stilSetari = 1;
int stiluriSetari = 2;
bool showmodeSetari = false;

#define charSize(textSize) (6*(textSize))

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC,TFT_RST);

void setup() {
  Serial.begin(9600);
  tft.begin();
  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  pinMode(okPin, INPUT_PULLUP);
  initVariables();
  tft.fillScreen(ILI9341_BLACK);
}

void initVariables(){
  screenWidth = tft.width();
  screenHeight = tft.height();
  partitieHeight = screenHeight/10;
  partitieWidth = screenWidth/14;
  highlightedMainMenu[highlightedCurrentMainMenu] = true;
  highlightedPozitieMenu[highlightedCurrentPozitieMenu] = true;
  highlightedSemnalizareMenu[highlightedCurrentSemnalizareMenu] = true;
}

void printInCenter(char* text, int textSize, int height, int textColor, bool highlighted){
  tft.setTextSize(textSize);
  uint16_t backColor = ILI9341_BLACK;
  uint16_t highColor = ILI9341_BLUE;
  int16_t textWidth = strlen(text) * 6 * textSize;
  uint16_t centerX = (screenWidth / 2) - (textWidth / 2);
  tft.setCursor(centerX, height);
  if(highlighted == 0){
    tft.setTextColor(textColor, backColor);
  }
  else{
    tft.setTextColor(textColor, highColor);
  }
  tft.println(text);
}

void printAtCursor(char* text, int textSize, int x, int y, int textColor, bool highlighted){
  tft.setTextSize(textSize);
  uint16_t backColor = ILI9341_BLACK;
  uint16_t highColor = ILI9341_BLUE;
  tft.setCursor(x, y);
  if(highlighted == 0){
    tft.setTextColor(textColor, backColor);
  }
  else{
    tft.setTextColor(textColor, highColor);
  }
  tft.println(text);
}

void procent(char* buffer, int valoare){
  // static char buffer[5];
  // char* buffer = (char*)malloc(5 * sizeof(char));
  if(valoare<100){
    sprintf(buffer, "%d%% ", valoare);
  }else{
    sprintf(buffer, "%d%%", valoare);
  }
  return buffer;
}

void loop() {
  
  switch(currentMenu){
    case 0: mainMenu(); break;
    case 1: pozitieMenu(); break;
    case 2: semnalizareMenu(); break;
    case 3: franaMenu(); break;
    case 4: setariMenu(); break;
  }
  
}

void setHighlight(bool highlighted[], int poz, int dimensiune){
  for(int i=0;i<dimensiune;i++){
    if(i==poz){
      highlighted[i]=true;
    }
    else{
      highlighted[i]=false;
    }
  }
}

void clearBackground() {
  int16_t x = 0; 
  int16_t y = 3*partitieHeight; 
  uint16_t bgColor = ILI9341_BLACK;
  int16_t textWidth = screenWidth;
  int16_t textHeight = 3*partitieHeight;
  tft.fillRect(x, y, textWidth, textHeight, bgColor);
}

void mainMenu(){
  if(firstStart==1){
    printInCenter("  Welcome!  ", 2, partitieHeight, ILI9341_WHITE, 0);
  }

  int lineLength = screenWidth;
  int lineThickness = 5;
  tft.fillRect(0, partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);
  tft.fillRect(0, 7.5*partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);
  int arrLength = 4;
  for(int i=0; i<arrLength; i++){
    if(highlightedMainMenu[i]){
      printAtCursor(modesMainMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 1);
    }
    else{
      printAtCursor(modesMainMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 0);
    }
  }
  printInCenter("    ", 2, 9*partitieHeight, ILI9341_WHITE, 0);
  // if(highlightedMainMenu[arrLength]==1){
  //   
  // }
  // else{
  //   printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 0);
  // }

  int up = digitalRead(upPin);
  int down = digitalRead(downPin);
  int ok = digitalRead(okPin);

  if(ok == 0){
    if(highlightedCurrentMainMenu == arrLength){//back
      //se duce inapoi in main menu
      // highlightedCurrentSemnalizareMenu = 0; // resetam la prima optiune
      // currentMenu=0;
      // clearBackground();
      // mainMenu(); 
    } 
    else{
      if(selected == 0){
        selected = 1;
      }else{
        selected = 0;
      }
    }
  }

  if(selected == 1){
    switch(highlightedCurrentMainMenu){
      case 0: {
        currentMenu=1;
        selected = 0;
        clearBackground();
        pozitieMenu(); 
        break;
      }
      case 1: {
        currentMenu=2;
        selected = 0;
        clearBackground();
        semnalizareMenu(); 
        break;
      }
      case 2: {
        currentMenu=3;
        selected = 0;
        clearBackground();
        franaMenu(); 
        break;
      }
      case 3: {
        currentMenu=4;
        selected = 0;
        clearBackground();
        setariMenu(); 
        break;
      }

    }
    // if(highlightedCurrentMainMenu == 0){ //pozitie
    //   currentMenu=1;
    //   selected = 0;
    //   clearBackground();
    //   pozitieMenu(); 
    // }
    // if(highlightedCurrentMainMenu == 1){ //semnalizare
    //   currentMenu=2;
    //   selected = 0;
    //   clearBackground();
    //   semnalizareMenu(); 
    // }
  }
  else{
    if(up == 0){
      if(highlightedCurrentMainMenu < arrLength-1){
        highlightedCurrentMainMenu+=1;
        
      }
    }
    if(down == 0){
      if(highlightedCurrentMainMenu > 0){
        highlightedCurrentMainMenu-=1;
      }
    }
  }
  setHighlight(highlightedMainMenu, highlightedCurrentMainMenu, arrLength);

}

void pozitieMenu(){
  printInCenter("  Pozitie  ", 2, partitieHeight, ILI9341_WHITE, 0);
  int lineLength = screenWidth;
  int lineThickness = 5;
  tft.fillRect(0, partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);
  tft.fillRect(0, 7.5*partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);

  int arrLength = 2;
  for(int i=0; i<arrLength; i++){
    if(highlightedPozitieMenu[i]){
      printAtCursor(modesPozitieMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 1);
    }
    else{
      printAtCursor(modesPozitieMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 0);
    }
  }
  char intensitate[5];
  int cnt=0;
  sprintf(intensitate, "%d%%", intensitatePozitie);
  printAtCursor(intensitate, 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  cnt=cnt+1;
  if(animatiePozitie){
    printAtCursor("ON ", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }else{
    printAtCursor("OFF", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }
  cnt=cnt+1;
  
  if(highlightedPozitieMenu[arrLength]==1){
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 1);
  }
  else{
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 0);
  }

  int up = digitalRead(upPin);
  int down = digitalRead(downPin);
  int ok = digitalRead(okPin);

  if(ok == 0){
    if(highlightedCurrentPozitieMenu == arrLength){//back to main
      highlightedCurrentPozitieMenu = 0; // resetam la prima optiune
      currentMenu=0;
      selected = 0;
      clearBackground();
      mainMenu(); 
    } 
    else{
      if(selected == 0){
        selected = 1;
      }else{
        selected = 0;
      }
    }
  }
  if(selected == 1){
    if(up == 0){
      if(highlightedPozitieMenu[0] == true){
        intensitatePozitie += 5;
      }
      if(highlightedPozitieMenu[1] == true){
        animatiePozitie = true;
      }
    } 
    if(down == 0){
      if(highlightedPozitieMenu[0] == true){
        intensitatePozitie -= 5;
      }
      if(highlightedPozitieMenu[1] == true){
        animatiePozitie = false;
      }
    }
  }
  else{
    if(up == 0){
      if(highlightedCurrentPozitieMenu < arrLength){
        highlightedCurrentPozitieMenu+=1;
      }
    }
    if(down == 0){
      if(highlightedCurrentPozitieMenu > 0){
        highlightedCurrentPozitieMenu-=1;
      }
    }
  }
  setHighlight(highlightedPozitieMenu, highlightedCurrentPozitieMenu, arrLength+1);
}

void semnalizareMenu(){
  printInCenter("  Semnalizare  ", 2, partitieHeight, ILI9341_WHITE, 0);
  int lineLength = screenWidth;
  int lineThickness = 5;
  tft.fillRect(0, partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);
  tft.fillRect(0, 7.5*partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);

  int arrLength = 4;
  for(int i=0; i<arrLength; i++){
    if(highlightedSemnalizareMenu[i]){
      printAtCursor(modesSemnalizareMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 1);
    }
    else{
      printAtCursor(modesSemnalizareMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 0);
    }
  }
  char intensitate[5];
  char viteza[5];
  char delay[5];
  procent(viteza, vitezaSemnalizare);
  procent(intensitate, intensitateSemnalizare);
  procent(delay, delaySemnalizare);
 
  int cnt=0;
  printAtCursor(intensitate, 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  cnt=cnt+1;
  printAtCursor(delay, 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  cnt=cnt+1;
  if(animatieSemnalizare){
    printAtCursor("ON ", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }else{
    printAtCursor("OFF", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }
  cnt=cnt+1;
  printAtCursor(viteza, 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  cnt=cnt+1;

  if(highlightedSemnalizareMenu[arrLength]==1){
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 1);
  }
  else{
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 0);
  }

  int up = digitalRead(upPin);
  int down = digitalRead(downPin);
  int ok = digitalRead(okPin);
 
  if(ok == 0){
    if(highlightedCurrentSemnalizareMenu == arrLength){//back to main
      highlightedCurrentSemnalizareMenu = 0; // resetam la prima optiune
      currentMenu=0;
      selected = 0;
      clearBackground();
      mainMenu(); 
    } 
    else{
      if(selected == 0){
        selected = 1;
      }else{
        selected = 0;
      }
    }
  }
  if(selected == 1){
    if(up == 0){
      if(highlightedSemnalizareMenu[0] == true){
        intensitateSemnalizare += 5;
      }
      if(highlightedSemnalizareMenu[1] == true){
        delaySemnalizare += 5;
      }
      if(highlightedSemnalizareMenu[2] == true){
        animatieSemnalizare = true;
      }
      if(highlightedSemnalizareMenu[3] == true){
        vitezaSemnalizare += 5;
      }
    } 
    if(down == 0){
      if(highlightedSemnalizareMenu[0] == true){
        intensitateSemnalizare -= 5;
      }
      if(highlightedSemnalizareMenu[1] == true){
        delaySemnalizare -= 5;
      }
      if(highlightedSemnalizareMenu[2] == true){
        animatieSemnalizare = false;
      }
      if(highlightedSemnalizareMenu[3] == true){
        vitezaSemnalizare -= 5;
      }
    }
  }
  else{
    if(up == 0){
      if(highlightedCurrentSemnalizareMenu < arrLength){
        highlightedCurrentSemnalizareMenu+=1;
      }
    }
    if(down == 0){
      if(highlightedCurrentSemnalizareMenu > 0){
        highlightedCurrentSemnalizareMenu-=1;
      }
    }
  }
  setHighlight(highlightedSemnalizareMenu, highlightedCurrentSemnalizareMenu, arrLength+1);
}

void franaMenu(){
  printInCenter("  Frana  ", 2, partitieHeight, ILI9341_WHITE, 0);
  int lineLength = screenWidth;
  int lineThickness = 5;
  tft.fillRect(0, partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);
  tft.fillRect(0, 7.5*partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);

  int arrLength = 2;
  for(int i=0; i<arrLength; i++){
    if(highlightedFranaMenu[i]){
      printAtCursor(modesFranaMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 1);
    }
    else{
      printAtCursor(modesFranaMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 0);
    }
  }

  char frecventa[5];
  int cnt=0;
  sprintf(frecventa, "%d%%", frecventaFrana);
  if(blinkFrana){
    printAtCursor("ON ", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }else{
    printAtCursor("OFF", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }
  cnt=cnt+1;
  printAtCursor(frecventa, 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  cnt=cnt+1;

  if(highlightedFranaMenu[arrLength]==1){
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 1);
  }
  else{
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 0);
  }

  int up = digitalRead(upPin);
  int down = digitalRead(downPin);
  int ok = digitalRead(okPin);

  if(ok == 0){
    if(highlightedCurrentFranaMenu == arrLength){//back to main
      highlightedCurrentFranaMenu = 0; // resetam la prima optiune
      currentMenu=0;
      selected = 0;
      clearBackground();
      mainMenu(); 
    } 
    else{
      if(selected == 0){
        selected = 1;
      }else{
        selected = 0;
      }
    }
  }
  if(selected == 1){
    if(up == 0){
      if(highlightedFranaMenu[0] == true){
        blinkFrana = true;
      }
      if(highlightedFranaMenu[1] == true){
        frecventaFrana += 5;
      }
    } 
    if(down == 0){
      if(highlightedFranaMenu[0] == true){
        blinkFrana = false;
      }
      if(highlightedFranaMenu[1] == true){
        frecventaFrana -= 5;
      }
    }
  }
  else{
    if(up == 0){
      if(highlightedCurrentFranaMenu < arrLength){
        highlightedCurrentFranaMenu+=1;
      }
    }
    if(down == 0){
      if(highlightedCurrentFranaMenu > 0){
        highlightedCurrentFranaMenu-=1;
      }
    }
  }
  setHighlight(highlightedFranaMenu, highlightedCurrentFranaMenu, arrLength+1);

}

void setariMenu(){

  printInCenter("  Setari  ", 2, partitieHeight, ILI9341_WHITE, 0);
  int lineLength = screenWidth;
  int lineThickness = 5;
  tft.fillRect(0, partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);
  tft.fillRect(0, 7.5*partitieHeight+charSize(2)+20, lineLength, lineThickness, ILI9341_BLUE);

  int arrLength = 2;
  for(int i=0; i<arrLength; i++){
    if(highlightedSetariMenu[i]){
      printAtCursor(modesSetariMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 1);
    }
    else{
      printAtCursor(modesSetariMenu[i], 2, partitieWidth, 3*partitieHeight + i*charSize(2) + i*10, ILI9341_WHITE, 0);
    }
  }
  char stil[5];
  int cnt=0;
  sprintf(stil, "%d%", stilSetari);
  printAtCursor(stil, 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  cnt=cnt+1;
  if(showmodeSetari){
    printAtCursor("ON ", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }else{
    printAtCursor("OFF", 2, 10.5*partitieWidth, 3*partitieHeight + cnt*charSize(2) + cnt*10, ILI9341_WHITE, 0);
  }
  cnt=cnt+1;
  
  if(highlightedSetariMenu[arrLength]==1){
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 1);
  }
  else{
    printInCenter("Back", 2, 9*partitieHeight, ILI9341_WHITE, 0);
  }

  int up = digitalRead(upPin);
  int down = digitalRead(downPin);
  int ok = digitalRead(okPin);

  if(ok == 0){
    if(highlightedCurrentSetariMenu == arrLength){//back to main
      highlightedCurrentSetariMenu = 0; // resetam la prima optiune
      currentMenu=0;
      selected = 0;
      clearBackground();
      mainMenu(); 
    } 
    else{
      if(selected == 0){
        selected = 1;
      }else{
        selected = 0;
      }
    }
  }
  if(selected == 1){
    if(up == 0){
      if(highlightedSetariMenu[0] == true){
        if(stilSetari < stiluriSetari)
        stilSetari += 1;
      }
      if(highlightedSetariMenu[1] == true){
        showmodeSetari = true;
      }
    } 
    if(down == 0){
      if(highlightedSetariMenu[0] == true){
        if(stilSetari > 1)
        stilSetari -= 1;
      }
      if(highlightedSetariMenu[1] == true){
        showmodeSetari = false;
      }
    }
  }
  else{
    if(up == 0){
      if(highlightedCurrentSetariMenu < arrLength){
        highlightedCurrentSetariMenu+=1;
      }
    }
    if(down == 0){
      if(highlightedCurrentSetariMenu > 0){
        highlightedCurrentSetariMenu-=1;
      }
    }
  }
  setHighlight(highlightedSetariMenu, highlightedCurrentSetariMenu, arrLength+1);


}