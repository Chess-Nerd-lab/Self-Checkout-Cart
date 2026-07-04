#include "Scale.h"

SecurityScale::SecurityScale() {
  currentBaggingWeight = 0.0;
  tolerance = 15.0; // Allow a +/- 15 gram deviation (packaging variations)
}

void SecurityScale::begin(int doutPin, int sckPin, float calFactor) {
  hx711.begin(doutPin, sckPin);
  hx711.set_scale(calFactor);
  hx711.tare();
  currentBaggingWeight = 0.0;
}

void SecurityScale::tareScale() {
  hx711.tare();
  currentBaggingWeight = 0.0;
}

float SecurityScale::getCurrentWeight() {
  if (hx711.is_ready()) {
    return hx711.get_units(3); // Average of 3 quick readings
  }
  return currentBaggingWeight; 
}

bool SecurityScale::waitForItem(float expectedAddedWeight) {
  float targetWeight = currentBaggingWeight + expectedAddedWeight;
  unsigned long startTime = millis();

  // Give the user 10 seconds to place the item
  while (millis() - startTime < 10000) {
    float reading = getCurrentWeight();

    // Check if the scale reading is near our target weight
    if (abs(reading - targetWeight) <= tolerance) {
      
      // Wait 500ms to ensure the weight is stable (not a hand pressing down)
      delay(500); 
      reading = getCurrentWeight();
      
      if (abs(reading - targetWeight) <= tolerance) {
        currentBaggingWeight = reading; // Update the baseline for the next item
        return true; 
      }
    }
    
    // Give background tasks time to breathe while waiting
    delay(50); 
  }
  
  return false; // Timed out
}