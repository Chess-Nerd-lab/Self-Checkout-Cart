#include "Scale.h"

SecurityScale::SecurityScale() {
  lastStableWeight = 0.0;
}

void SecurityScale::begin(int doutPin, int sckPin, float calFactor) {
  hx711.begin(doutPin, sckPin);
  hx711.set_scale(calFactor);
  // We do NOT tare here. Tare is called in main.ino AFTER startup check.
}

long SecurityScale::getRawValue() {
  if (hx711.is_ready()) {
    return hx711.read(); 
  }
  return 0;
}

void SecurityScale::tareScale() {
  hx711.tare();
  lastStableWeight = 0.0;
}

float SecurityScale::getCurrentWeight() {
  if (hx711.is_ready()) {
    // Read 1 sample to keep it fast and non-blocking
    lastStableWeight = hx711.get_units(5); 
  }
  return lastStableWeight; 
}

float SecurityScale::getTolerance(float itemWeight) {
  float tol = abs(itemWeight) * 0.05; // 5% dynamic
  return (tol > 12.0) ? tol : 12.0;     // 12g minimum floor
}