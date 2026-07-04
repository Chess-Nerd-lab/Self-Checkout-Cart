#ifndef SYSTEMIO_H
#define SYSTEMIO_H

#include <Arduino.h>
#include "modules.h"

class SystemIO {
  private:
    unsigned long lastVoidPress;
    unsigned long lastCheckoutPress;
    const unsigned long debounceDelay = 250; // Prevents double-triggers

  public:
    SystemIO();
    void begin();
    
    // Feedback signals
    void successSignal();
    void errorSignal();
    
    // Input reading
    bool isVoidPressed();
    bool isCheckoutPressed();
};

#endif