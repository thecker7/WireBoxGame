#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 4
#define DIO 2

//Buzzer Pin
#define BUZZ 9

//LED Pins
#define REDL 5
#define YELL 3
#define GRNL 6

//WIRE Pins
#define YELW 8
#define GRNW 12
#define BLUEW 11
#define REDW 10

// Change to speed up or slow down
#define DLY_MS   1000
// 0b0GFEDCBA
//   A
//F     B
//   G
//E     C
//   D
const uint8_t CH_A = 119;
const uint8_t CH_B = 0b01111100;
const uint8_t CH_C = 57;
const uint8_t CH_D = 94;
const uint8_t CH_E = 121;
const uint8_t CH_F = 113;
const uint8_t CH_G = 0b00111101;
const uint8_t CH_H = 118;
const uint8_t CH_I = 48;
const uint8_t CH_J = 30;
const uint8_t CH_L = 56;
const uint8_t CH_N = 84;
const uint8_t CH_O = 92;
const uint8_t CH_P = 115;
const uint8_t CH_R = 80;
const uint8_t CH_S = 109;
const uint8_t CH_T = 120;
const uint8_t CH_U = 62;
const uint8_t CH_Y = 110;
const uint8_t CH_Z = 95;
const uint8_t CH_DASH = 64;

const uint8_t WORD_UHOH[] = { CH_U, CH_H, CH_O, CH_H  };

char wordlist[][4] = { "xnox", //0
                    "nope",
                    "xyes",
                    "yupx",
                    "high",
                    "seex",   //5
                    "seax",
                    "deer",
                    "dear",
                    "allx",
                    "ohno",   //10
                    "call",
                    "arcx",
                    "huhx",
                    "uhuh",
                    "hoox",   //15
                    "toox",
                    "xtox",
                    "xyou",
                    "xxux",
                    "hard",   //20
                    "fair",
                    "fare",
                    "butx",
                    "butt",
                    "bare",   //25
                    "bear",
                    "shoe",
                    "xbex",
                    "beex",
                    "xohx",   //30
                    "xnot",
                    "tube",
                    "tuba",
                    "dead",
                    "cool",   //35
                    "soup",
                    "xupx",
                    "east",
                    "easy",
                    "pool",   //40
                    "beep",
                    "deep",
                    "xhix",
                    "feel" };
    //Condition: Beeping, Words, RBLINK, YBLINK, GBLINK, RLED, YLED, GLED
    //State: RBGY
const uint8_t MODEMATRIX[16][8] = { { 0, 0, 0, 0,0,0,0,0}, //0b0000, all cut, endgame mode
                                    { 0b0000,  0b10000101,  0b0000, 0b11000011,  0b0000,  0b01001110,  0b1001,  0b11001101},   //0b0001
                                    { 0b0000,  0b01000011,  0b0000, 0b11000001,  0b0000,  0b11000111,  0b0110,  0b11001000},   //0b0010
                                    { 0b0001,  0b10111001,  0b0001, 0b10000111,  0b0010,  0b10000110,  0b0010,  0b00000101},   //0b0011
                                    { 0b0000,  0b11000010,  0b0000, 0b11011101,  0b0000,  0b11010010,  0b0101,  0b11001110},   //0b0100
                                    { 0b0100,  0b00111011,  0b0100, 0b10101001,  0b0001,  0b00111111,  0b0001,  0b10000010},   //0b0101
                                    { 0b0010,  0b10000010,  0b0010, 0b01010101,  0b0100,  0b11000111,  0b0100,  0b10000110},   //0b0110
                                    { 0b0110,  0b10001010,  0b0110, 0b10000110,  0b0101,  0b11010011,  0b0011,  0b10111111},   //0b0111
                                    { 0b0000,  0b10000100,  0b0000, 0b11010110,  0b0000,  0b01001101,  0b1001,  0b01000001},   //0b1000
                                    { 0b1000,  0b11000011,  0b1000, 0b10000110,  0b0001,  0b01000000,  0b0001,  0b11010001},   //0b1001
                                    { 0b0010,  0b00000011,  0b0010, 0b00000100,  0b1000,  0b01000011,  0b1000,  0b11000111},   //0b1010
                                    { 0b1010,  0b10000100,  0b1010, 0b10000011,  0b1001,  0b00000110,  0b0011,  0b00100010},   //0b1011 no loop back
                                    { 0b1000,  0b00111111,  0b1000, 0b10000010,  0b0100,  0b00100001,  0b0100,  0b10010100},   //0b1100
                                    { 0b1100,  0b00111010,  0b1100, 0b10000101,  0b1001,  0b10111111,  0b0101,  0b10000100},   //0b1101
                                    { 0b1100,  0b10000111,  0b1100, 0b10011000,  0b1010,  0b10110000,  0b0110,  0b10011010},   //0b1110
                                    { 0b1110,  0b10100010,  0b1101, 0b10000111,  0b1011,  0b10000001,  0b0111,  0b10111100} }; //0b1111 

const uint8_t BEEPING   =   0b10000000;
const uint8_t WORDMODE  =   0b01000000;
const uint8_t R_BLINK   =   0b00100000;
const uint8_t Y_BLINK   =   0b00010000;
const uint8_t G_BLINK   =   0b00001000;
const uint8_t R_ON      =   0b00000100;
const uint8_t Y_ON      =   0b00000010;
const uint8_t G_ON      =   0b00000001;

int cdown = 60;   //timer in seconds
int strikes = 4;  //easy mode
int win = 0;      //win status check
int state = 0;
int prevState = 0;
int nextState = 0b1111;
const int tick_tone = 1000;
int fail_tone = 100;
uint8_t condition = 0b10100000;
long dice=0; //for randomization

int blink_interval = 250;
int tick_interval = 1000;
int prev_blink = 0;
int prev_tick = 0;
int currentMillis=0;

uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };    //general screen register to manipulate
uint8_t strikes_display[] = { CH_O, CH_O, CH_O, CH_O };
uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };

TM1637Display display(CLK, DIO);

int checkState(void) {
  int state = 0;
  state = (digitalRead(YELW) | digitalRead(GRNW) << 1 | digitalRead(BLUEW) << 2 | digitalRead(REDW) << 3);
  return ~state & 0b00001111; //invert state because HIGH=disconnected and LOW=connected
}

void ShowText(char kword[]){
// char kword[] throws a warning upon compiling. not sure why but it still seems to work
  uint8_t array_to_load[4];
  uint8_t toLoad;
  for(int i=0; i<4; i++){
    toLoad = 0;
    switch (kword[i]){
        case 'a':
          toLoad=CH_A;
          break;
        case 'b':
          toLoad=CH_B;
          break;
        case 'c':
          toLoad=CH_C;
          break;
        case 'd':
          toLoad=CH_D;
          break;
        case 'e':
          toLoad=CH_E;
          break;
        case 'f':
          toLoad=CH_F;
          break;
        case 'g':
          toLoad=CH_G;
          break;
        case 'h':
          toLoad=CH_H;
          break;
        case 'i':
          toLoad=CH_I;
          break;
        case 'j':
          toLoad=CH_J;
          break;
        case 'l':
          toLoad=CH_L;
          break;
        case 'n':
          toLoad=CH_N;
          break;
        case 'o':
          toLoad=CH_O;
          break;
        case 'p':
          toLoad=CH_P;
          break;
        case 'r':
          toLoad=CH_R;
          break;
        case 's':
          toLoad=CH_S;
          break;
        case 't':
          toLoad=CH_T;
          break;
        case 'u':
          toLoad=CH_U;
          break;
        case 'y':
          toLoad=CH_Y;
          break;
        case 'z':
          toLoad=CH_Z;
          break;
    }
    array_to_load[i]=toLoad;
  }
  display.setSegments(array_to_load);
}

void youWin(void) {
  while(1) {
    ShowText("good");
    delay(DLY_MS);
    ShowText("xjob");
    delay(DLY_MS);
    for (int i=0; i<10; i++) {
      display.setSegments(blank);
      delay(100);
      showTime(cdown);
      delay(600);
    }
  }
}

void gameOver(void) {

    for (int i=0; i<4; i++){
      data[i] = WORD_UHOH[i];
    }
    display.setSegments(data);
    delay(2000);
    for (int i=0; i<28; i++){
      tone(BUZZ, fail_tone, 30);
      fail_tone -= 2;
      data[i%4] &= ~(1<<(i%7));
      display.setSegments(data);
      delay(30);
    }
    tone(BUZZ, fail_tone, 1000);
    delay(2000);
    while(1){ //stop here
      ShowText("xtry");
      delay(1000);
      ShowText("agai");
      delay(500);
      ShowText("gain");
      delay(600);
    }
}

void endGame(void){
  uint8_t seq[] = { 0b1000, 0b0001, 0b0010, 0b0100 }; //sequence of wires to be reconnected
  //shuffle the order of wires
  for (int i=0; i<10; i++){
    int a = random(0,4);  //pick two random values in the array
    int b = random(0,4);
    int t = seq[a];       //and swap them
    seq[a]=seq[b];
    seq[b]=t;
  }

  for (int i=0; i<4; i++){
    if (seq[i]==0b1000){
      data[i]=CH_R;
    }
    if (seq[i]==0b0100){
      data[i]=CH_B;;
    }
    if (seq[i]==0b0010){
      data[i]=CH_G;
    }
    if (seq[i]==0b0001){
      data[i]=CH_Y;
    }
  }
  
  display.setSegments(data);
  prevState=0; //expect this since endgame is triggered by 0b0000 state
  nextState=0;
  for (int i=0; i<4; i++){
    if (i >0){
      ShowText("xxxx"); //hide the display after the first wire is connected
    }
    nextState |= seq[i];
    prevState = state;
      while (prevState==state){
        //display.showNumberDec(nextState);
        state = checkState();
        tone(BUZZ, random(500,1200), random(100,500));
        delay(500);
      }
    if (state != nextState) {//oops
        return; //add a strike here?
    }
  }
  youWin();  //if you've made it here then you have won
}

void showTime(int n_seconds){
  display.showNumberDecEx(((n_seconds/60)*100)+(n_seconds%60),(n_seconds%2)*64);
}

void setup()
{
  //screen setup
  display.setBrightness(0x0f);


  //initialize LED pins
  pinMode(REDL,OUTPUT);
  pinMode(YELL,OUTPUT);
  pinMode(GRNL,OUTPUT);

  pinMode(BUZZ, OUTPUT);
  for (int i=0; i<20; i++) {
    tone(BUZZ, random(50,190), 50);
    delay(50);
  }

  //initialize wire pins
  pinMode(REDW,INPUT_PULLUP); //remember this makes them LOW when
connected and HIGH when disconnected
  pinMode(YELW,INPUT_PULLUP);
  pinMode(BLUEW,INPUT_PULLUP);
  pinMode(GRNW,INPUT_PULLUP);

  //set difficulty
  if (digitalRead(REDW)){
    cdown+=30;
  }
  if (digitalRead(YELW)){
    cdown+=30;
  }
  if (digitalRead(BLUEW)){
    cdown+=30;
  }
  if (digitalRead(GRNW)){
    cdown+=30;
  }
  if ((cdown <61)||(cdown>156)){
    ShowText("xtoo");
    delay(500);
  }
  if (cdown < 119){
    ShowText("hard");
  }
  else{
    ShowText("easy");
  }
  delay(2500);

  //wait for wires to be connected
  while (digitalRead(REDW) || digitalRead(YELW) || digitalRead(BLUEW)
|| digitalRead(GRNW)){
    tone(BUZZ, 100, 100);
    ShowText("conn");
    delay(500);
    display.setSegments(blank);
    delay(550);
  }


  ShowText("good");
  delay(550);
  tone(BUZZ, tick_tone,100);
  //check that LEDS are working
  ShowText("rdyx");
  for (int i=0; i<255; i++){
    analogWrite(REDL,i);
    delay(5);
  }

  tone(BUZZ, tick_tone,100);
  digitalWrite(REDL,LOW);
  ShowText("xset");
  for (int i=0; i<255; i++){
    analogWrite(YELL,i);
    delay(5);
  }

  tone(BUZZ, tick_tone,100);
  digitalWrite(YELL,LOW);
  ShowText("xgox");
  for (int i=0; i<255; i++){
    analogWrite(GRNL,i);
    delay(5);
  }

  tone(BUZZ, tick_tone +200 ,100);
  //randomize
  randomSeed(analogRead(0));

  digitalWrite(GRNL,LOW);
  //display.showNumberDecEx(100,64); //64 enables colon
  delay(DLY_MS);
}



void loop()
{


  //check state
  prevState = state;
  state = checkState();

  if (state==0) {
    endGame();
  }

  if (state != prevState) {//state has changed

    if (state != nextState) {   //a mistake was made
      strikes -= 1;
      tone(BUZZ, fail_tone, 500);
      ShowText("oops");
      delay(DLY_MS);
      display.setSegments(strikes_display);
      strikes_display[strikes]= CH_DASH;
      delay(250);
      display.setSegments(strikes_display);
      delay(550);
      if (strikes==0){
        gameOver();
      }
     }
     else {
      if (state != 0b1111) {
        ShowText("good");
        tone(BUZZ, tick_tone, 50);
        delay(50);
        tone(BUZZ, tick_tone+100, 50);

        delay(1000);
        cdown+=5;
      }

     }




    //Begin handling state transitions
    //RBGY
    //save yourself from calling random a bajillion times
    dice=random(0,4);
    nextState=MODEMATRIX[state][dice*2];
    condition=MODEMATRIX[state][(dice*2)+1];
  }


  //timekeeping
  currentMillis = millis();

  //LED handling
  if (!(condition & WORDMODE)) {
    if ((currentMillis - prev_blink) > blink_interval){
      if (condition & R_BLINK){
        condition ^= R_ON;
      }
      if (condition & G_BLINK){
        condition ^= G_ON;
      }
      if (condition & Y_BLINK){
        condition ^= Y_ON;
      }
      digitalWrite(REDL, (condition & R_ON));
      digitalWrite(YELL, (condition & Y_ON));
      digitalWrite(GRNL, (condition & G_ON));
      prev_blink = currentMillis;
    }
  }
  else {
      digitalWrite(REDL, LOW);
      digitalWrite(YELL, LOW);
      digitalWrite(GRNL, LOW);
  }
  //begin countdown
  if ((currentMillis - prev_tick) > tick_interval){

    if (condition & WORDMODE){
      ShowText(wordlist[condition & 0b00111111]);
    }
    else {
      showTime(cdown);
      if (cdown < 0){
      gameOver();
      }
      cdown -= 1;
    }
    if (condition & BEEPING) {
      tone(BUZZ,tick_tone,50);
    }
    prev_tick=currentMillis;
  }

  delay(10); //leftover from Serial debugging
}
