#include "SystemIO.h"

SystemIO::SystemIO() {
  lastViewPress = 0;
  checkoutBtnState = 0;
  singleClickFlag = false;
  doubleClickFlag = false;
}

void SystemIO::begin() {
  pinMode(PIN_BTN_VIEW, INPUT_PULLUP);
  pinMode(PIN_BTN_CHECKOUT, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  alarmOff();
}

void SystemIO::alarmOn() {
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
}

void SystemIO::alarmOff() {
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}

void SystemIO::successSignal() {
  alarmOn(); delay(200); alarmOff();
}

void SystemIO::errorSignal() {
  for(int i=0; i<3; i++) { alarmOn(); delay(100); alarmOff(); delay(100); }
}

void SystemIO::update() {
  // Non-blocking Checkout Double-Click Logic
  bool pressed = (digitalRead(PIN_BTN_CHECKOUT) == LOW);
  
  switch(checkoutBtnState) {
    case 0: // Idle
      if (pressed) { checkoutTimer = millis(); checkoutBtnState = 1; }
      break;
    case 1: // Wait release
      if (!pressed) { checkoutTimer = millis(); checkoutBtnState = 2; }
      break;
    case 2: // Wait for second click
      if (pressed) { checkoutBtnState = 3; } // Double click!
      else if (millis() - checkoutTimer > 300) { singleClickFlag = true; checkoutBtnState = 0; } // Timeout, single click
      break;
    case 3: // Wait second release
      if (!pressed) { doubleClickFlag = true; checkoutBtnState = 0; }
      break;
  }
}

bool SystemIO::isViewPressed() {
  if (digitalRead(PIN_BTN_VIEW) == LOW) {
    if (millis() - lastViewPress > 300) {
      lastViewPress = millis();
      return true;
    }
  }
  return false;
}

bool SystemIO::isCheckoutSingleClicked() {
  if (singleClickFlag) { singleClickFlag = false; return true; }
  return false;
}

bool SystemIO::isCheckoutDoubleClicked() {
  if (doubleClickFlag) { doubleClickFlag = false; return true; }
  return false;
}