#include "SystemIO.h"

SystemIO::SystemIO() {
  lastVoidPress = 0;
  lastCheckoutPress = 0;
}

void SystemIO::begin() {
  // Use internal pullup resistors so buttons trigger when connected to GND
  pinMode(PIN_BTN_VOID, INPUT_PULLUP);
  pinMode(PIN_BTN_CHECKOUT, INPUT_PULLUP);
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}

void SystemIO::successSignal() {
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
  delay(200); // Short, pleasant beep
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}

void SystemIO::errorSignal() {
  // Three rapid flashes and beeps for an error state
  for(int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    delay(100);
  }
}

bool SystemIO::isVoidPressed() {
  if (digitalRead(PIN_BTN_VOID) == LOW) {
    if (millis() - lastVoidPress > debounceDelay) {
      lastVoidPress = millis();
      return true;
    }
  }
  return false;
}

bool SystemIO::isCheckoutPressed() {
  if (digitalRead(PIN_BTN_CHECKOUT) == LOW) {
    if (millis() - lastCheckoutPress > debounceDelay) {
      lastCheckoutPress = millis();
      return true;
    }
  }
  return false;
}