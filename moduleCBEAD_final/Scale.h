#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>
#include <HX711.h>
#include "modules.h"

class SecurityScale {
  private:
    HX711 hx711;
    float lastStableWeight;

  public:
    SecurityScale();
    void begin(int doutPin, int sckPin, float calFactor);
    void tareScale();
    long getRawValue();
    float getCurrentWeight();
    float getTolerance(float itemWeight);
};

#endif