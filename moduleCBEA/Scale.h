#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>
#include <HX711.h>
#include "modules.h"

class SecurityScale {
  private:
    HX711 hx711;
    float currentBaggingWeight;
    float tolerance; 

  public:
    SecurityScale();
    void begin(int doutPin, int sckPin, float calFactor);
    void tareScale();
    float getCurrentWeight();
    
    // Halts the system until the expected weight is placed on the scale
    // Returns true if successful, false if it times out or detects a mismatch
    bool waitForItem(float expectedAddedWeight);
};

#endif