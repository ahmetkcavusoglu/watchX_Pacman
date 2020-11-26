/*
   Pongclock Code for 0miker0's Pong Clock.
   Code by mic159.

   FOR HARDWARE REVISION 2.3!
   Earlier revisions should use 2.2 branch.

   Requirements:
   Adafruit GFX https://github.com/adafruit/Adafruit-GFX-Library
   Adafriut SSD1306 https://github.com/adafruit/Adafruit_SSD1306
   RTClib https://github.com/mic159/RTClib
*/

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EnableInterrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "Buttons.h"
#include "Menu.h"
#include "Menu_Settings.h"
#include "Menu_Settings_24.h"
#include "Menu_Settings_Time.h"
#include "Menu_Settings_Date.h"
#include "Menu_Settings_Brightness.h"
#include "Menu_Clockface.h"
#include "State.h"

#define NEXT_PIN   10
#define SELECT_PIN 8
#define PREV_PIN 11

#define OLED_DC     A3
#define OLED_CS     A5
#define OLED_RESET  A4
#define WIDTH      128
#define HEIGHT     64

// Set this to enable printing debug stats to the screen
//#define DEBUG_STATS

Button btnNext(NEXT_PIN);
Button btnSelect(SELECT_PIN);
Button btnPrev(PREV_PIN);
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
RTC_DS1307 RTC;

#define SSD1306_LCDHEIGHT 64

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void buttonNextPressed() {
  btnNext.interrupt();
}
void buttonSelectPressed() {
  btnSelect.interrupt();
}
void buttonPrevPressed() {
  btnPrev.interrupt();
}

State state;
Menu* menu = NULL;

int lastButtonState = 0;  
int count = 0;
int statee = 0;
int progMode=1;

bool IsUSBConnected() {
  return (USBSTA&(1<<VBUS));
}

void interruptFunction() {
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void sleepNow()
{
  lastButtonState=LOW;
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  power_adc_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  enableInterrupt(PREV_PIN, interruptFunction, CHANGE);
  sleep_cpu();
  sleep_mode();            
  sleep_disable();   
  disableInterrupt(PREV_PIN); 
  power_adc_enable();
}

void setSleepStatusBasedonUSB() {
  if (IsUSBConnected()) { // check if USB is connected.
    progMode=10;
    if (digitalRead(PREV_PIN) == 0) {
      display.ssd1306_command(SSD1306_DISPLAYON);
    }
  } else {
    if (progMode > 0){
      progMode=progMode-1;
    }
    if (count >= 6500) {
      delay(500);     // this delay is needed, the sleep
      count = 0;
      sleepNow();     // sleep function called here
    }
  }
}

void sleep_watch(){

  int buttonState = digitalRead(PREV_PIN);

  if (buttonState == LOW && lastButtonState==HIGH) {
    //&& buttonState != lastButtonState
    statee = statee+1;

    if (statee > 2){
      statee = 0;
    }
    delay(50);
  }
  lastButtonState = buttonState;

//  display.clearDisplay();
  display.display();

  setSleepStatusBasedonUSB();
  count++;
  count = count % 10000;
}

void setup(void) {
  Serial.begin(9600);
  randomSeed(analogRead(A3));
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setRotation(0);

  // First time init, set to code compile date.
  if (!RTC.isrunning()) {
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Setup buttons
  pinMode(NEXT_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(PREV_PIN, INPUT_PULLUP);
  //attachInterrupt(0, buttonSelectPressed, LOW);
  //attachInterrupt(1, buttonNextPressed, LOW);

  // Splash
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 20);
  display.print(F("Pacman Clock"));
  display.setCursor(66, 48);
  display.print(F("by 0miker0"));
  display.setCursor(78, 56);
  display.print(F("& mic159"));
  display.display();
  delay(2000);

  // Load things from state
  display.dim(state.dim);

  state.update();
  switchMenu(MENU_CLOCK);
}

void loop() {
    sleep_watch();
  // As an optimisation, we only draw the display
  // when we really need to. Drawing the display
  // every time is wasteful if nothing has changed.
  bool draw = false;
#ifdef DEBUG_STATS
  unsigned long timer = millis();
#endif

  // Buttons
  if (btnNext.update() && btnNext.read()) {
    menu->button1();
    draw = true;
  }
  if (btnSelect.update() && btnSelect.read()) {
    menu->button2();
    draw = true;
  }
  if (btnPrev.update() && btnPrev.read()) {
    menu->button3();
    draw = true;
  }

  // Switch menu if indicated.
  updateMenuSelection();

  // Update
  state.update();
  if (menu->update()) {
    draw = true;
  }

  // Display
  if (draw) {
    display.clearDisplay();
    menu->draw(&display);

#ifdef DEBUG_STATS
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(80, HEIGHT - 10);
    display.print(freeRam());
    display.setCursor(109, HEIGHT - 10);
    display.print(millis() - timer);
#endif

    display.display();
  }

}

#ifdef DEBUG_STATS
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif
