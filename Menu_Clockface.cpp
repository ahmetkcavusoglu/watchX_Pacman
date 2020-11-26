#include <Arduino.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include "Menu_Clockface.h"
#include "Menu.h"
#include "State.h"
#include "Clockface_Pacman.h"

ClockfaceMenu::ClockfaceMenu()
: Menu(MENU_CLOCK)
, faceType(FACE_PACMAN)
, face(NULL)
{
  faceType = state.current_face;
  if (faceType >= FACE_MAX) faceType = 0;
  changeMenu();
  uint8_t hour = state.now.hour();
  if (!state.mode24h && hour > 12) {
    hour = hour - 12;
  }
  face->begin(hour, state.now.minute());
}

ClockfaceMenu::~ClockfaceMenu() {
  delete face;
}

bool ClockfaceMenu::update() {
  uint8_t hour = state.now.hour();
  if (!state.mode24h && hour > 12) {
    hour = hour - 12;
  }
  face->update(hour, state.now.minute());
  // Always render
  return true;
}

void ClockfaceMenu::draw(Adafruit_GFX* display) const {
  face->draw(display);
}

void ClockfaceMenu::button1() {
  switchMenu(MENU_SETTINGS);
}

void ClockfaceMenu::button2() {
  switchMenu(MENU_SETTINGS);
}

void ClockfaceMenu::button3() {
  switchMenu(MENU_SETTINGS);
}


void ClockfaceMenu::changeMenu() {
  // Switch object
  if (face) {
    delete face;
    face = NULL;
  }
  switch(faceType) {
  case FACE_PACMAN:
      face = new ClockfacePacman();
    break;
  }

  // Call begin
  uint8_t hour = state.now.hour();
  if (!state.mode24h && hour > 12) {
    hour = hour - 12;
  }
  face->begin(hour, state.now.minute());

  state.current_face = faceType;
  state.save();
}
