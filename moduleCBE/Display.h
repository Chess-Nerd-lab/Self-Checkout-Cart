#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "modules.h"

class SystemDisplay {
  private:
    // 0x27 is the most common I2C address for these backpacks. 
    // If your screen is blank, it might be 0x3F.
    LiquidCrystal_I2C lcd; 

  public:
    SystemDisplay();
    
    void begin();
    void printBootStep(uint8_t row, String message);
    void setupActiveUI();
    void updateCart(Item item, float currentTotal);
    void showError();
};

#endif