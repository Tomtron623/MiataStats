#include <Arduino.h>
#include <OBD2UART.h>
#include <U8g2lib.h>
#include <RunningAverage.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_SW_SPI
#include <Wire.h>
#endif
//define screen
U8G2_SSD1322_NHD_256X64_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 12, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
#define red  2
//placeholder variables, will be updated with actual values by obd
//psi has to be converted from BAR (raw value)
COBD obd;
RunningAverage GALAV(10);
int psi = 29;
int range = 200;
//button press counter
int changeButton = 4;
int i = 0;
int v = 2;
int screen = 4;
int CMPG = 23;
float GAL = 10;
int temp = 29;
int ENGTMP = 60;
int engld = 50;
int rpm = 720;

void bootup() {
  u8g2.setFont(u8g2_font_t0_13_tf);
  u8g2.setCursor(10,10);
  u8g2.print("Booting up..");
  u8g2.setCursor(100,10);
  u8g2.print("(X) Enabling Vroom");
  u8g2.setCursor(100,25);
  u8g2.print("(X) Cool Driver");
  u8g2.setCursor(100,40);
  u8g2.print("(X) Passenger Princess");
  delay(300);
}
void staticUI() {
  u8g2.setCursor(10,10);
  u8g2.setFont(u8g2_font_inb16_mf );
  u8g2.print(range);
  u8g2.drawRFrame(0, 0, 64, 64, 3);
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.setCursor(10,35);
  u8g2.print("Rng");
  u8g2.drawRFrame(252, 0, 4, 15, 2);
  u8g2.drawRFrame(252, 16, 4, 15, 2);
  u8g2.drawRFrame(252, 32, 4, 15, 2);
  u8g2.drawRFrame(252, 48, 4, 15, 2);
}
void currentScreen() {
  if(screen == 1) {
  screen1();
  } else if (screen == 2) {
  screen2();
  } else if (screen == 3) {
  screen3();
  } else if (screen == 4){
   screen4();
  }
}
void screen1(){
   //front tires
  u8g2.drawRFrame(73, 0, 32, 64, 10);
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.setCursor(75,22);
  u8g2.print(psi);
  u8g2.setCursor(105,22);
  u8g2.print("F");
  u8g2.drawRFrame(119, 0, 32, 64, 10);
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.setCursor(121,22);
  u8g2.print(psi);
  //rear tires
  u8g2.drawRFrame(163, 0, 32, 64, 10);
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.setCursor(165,22);
  u8g2.print(psi);
  u8g2.setCursor(196,22);
  u8g2.print("R");
  u8g2.drawRFrame(210, 0, 32, 64, 10);
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.setCursor(212,22);
  u8g2.print(psi);
  //selector indicator
  u8g2.drawRFrame(253, 0, 2, 14, 0);
}
void screen2(){
  u8g2.setCursor(66,45);
  u8g2.print("MPG");
  u8g2.setCursor(66,25);
  u8g2.print(CMPG);
  u8g2.setCursor(120,45);
  u8g2.print("Tank");
  u8g2.setCursor(120,25);
  u8g2.print(GAL, 1);
  u8g2.setCursor(170,0);
  u8g2.print(temp);
  u8g2.setCursor(195,0);
  u8g2.print("'C");
  u8g2.drawRFrame(253, 17, 2, 14, 0);
}
void screen3 (){
  u8g2.setCursor(66,7);
  u8g2.print("ENGTemp");
  u8g2.setCursor(180,7);
  u8g2.print(ENGTMP);
  u8g2.setCursor(205,5);
  u8g2.print("'C");
  u8g2.setCursor(66,37);
  u8g2.print("EngLoad");
  u8g2.setCursor(180,37);
  u8g2.print(engld);
  u8g2.setCursor(225,37);
  u8g2.print("%");
  //selector indicator
  u8g2.drawRFrame(253, 33, 2, 14, 0);
}
void screen4 (){
  int x = 66;
  int y = 64;
  for (int i = 0; i <= (rpm/100); i++) {
    int x = 66 + (i*2);
    int y = 63;
    u8g2.drawRBox(x, y-(i/1.35), i*2,(i/1.35), 0);
    u8g2.drawRFrame(253, 49, 2, 14, 0);
  }
}
void buttons() {
  if (digitalRead(changeButton)== LOW) {
  }

}

void draw() {
  u8g2.firstPage();
  do {
    buttons();
    staticUI();
    currentScreen();
  } while ( u8g2.nextPage() );
}

void valueReset() {
  int value;
  if (screen == 1) {
    
  } else if (screen == 2) {
    if(obd.readPID(PID_AMBIENT_TEMP, value)) {
      temp = value;
      i = 5;
    
  } else if (screen == 3) {
    if(obd.readPID(PID_ENGINE_LOAD, value)) {
      engld = value;
      i = 5;
    }
    if(obd.readPID(PID_COOLANT_TEMP, value)) {
      ENGTMP = value;
      i = 5;
    }
  } else if (screen == 4) {
    if(obd.readPID(PID_RPM, value)) {
      rpm = value;
      i = 5;
    }
  }
}
}

void setup(void) {
  pinMode(changeButton, INPUT_PULLUP);
  u8g2.begin();
  u8g2.setFontPosTop();
  GALAV.clear();
  pinMode(red, OUTPUT);
  digitalWrite(red, HIGH);
  u8g2.firstPage();
  do {
    bootup();
  } while ( u8g2.nextPage() );
  delay(2000);
  obd.begin();
  // initiate OBD-II connection until success
  while (!obd.init());
}



void loop() {
  valueReset();
  int value;
  if(obd.readPID(PID_FUEL_LEVEL, value)) {
      GAL = (value*12.7)/100;
      GALAV.addValue(GAL);
      GAL = GALAV.getAverage();
      range = (GAL*25);
      i = 5;
  }
  if(v != i){
  i = v;
  draw();
  }
  delay(1);
  //draw();
}
