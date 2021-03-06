#ifndef MENU_CLOCKFACE_H
#define MENU_CLOCKFACE_H

#include <Arduino.h>
#include <RTClib.h>
#include "Menu.h"
#include "Clockface.h"


enum FACE {
  
  FACE_PACMAN,

  FACE_MAX
};

class ClockfaceMenu : public Menu {
public:
  ClockfaceMenu();
  ~ClockfaceMenu();

  bool update();
  void draw(Adafruit_GFX* display) const;
  void button1();
  void button2();
  void button3();
private:
  void changeMenu();
  Clockface* face;
  uint8_t faceType;
};

#endif
