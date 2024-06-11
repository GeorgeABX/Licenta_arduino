int led = 9;
int cnt=0;
int brt=0;
void setup() {
  pinMode(led,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void ledRosu(){
  digitalWrite(led,HIGH);
  delay(500);
  digitalWrite(led,LOW);
  delay(500);
}

void ledIn(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second
}

void loop() {
//  if(cnt==1){
//    ledRosu();
//    delay(500);
//    cnt=0;
//  }
//  else{
//    ledIn();
//    delay(500);
//    cnt++;
//  }
  for(int i=0;i<255;i++){
    analogWrite(led, i);
    delay(100);
  }
  for(int i=254;i>=0;i--){
    analogWrite(led, i);
    delay(100);
  }
}
