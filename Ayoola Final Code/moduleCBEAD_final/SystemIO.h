#ifndef SYSTEMIO_H
#define SYSTEMIO_H

#include <Arduino.h>
#include "modules.h"

class SystemIO {
  private:
    unsigned long lastViewPress;
    
    // Double click variables
    int checkoutBtnState;
    unsigned long checkoutTimer;
    bool singleClickFlag;
    bool doubleClickFlag;

  public:
    SystemIO();
    void begin();
    void update(); // Must be called in loop()
    
    void successSignal();
    void errorSignal();
    void alarmOn();
    void alarmOff();
    
    bool isViewPressed();
    bool isCheckoutSingleClicked();
    bool isCheckoutDoubleClicked();
};

#endif