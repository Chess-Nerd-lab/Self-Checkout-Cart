#ifndef MODULES_H
#define MODULES_H

#include <Arduino.h>

// Hardware Pin Definitions
#define HX711_DOUT_PIN 4
#define HX711_SCK_PIN 5
#define SCALE_CAL_FACTOR -200.0 // Assuming you adjusted to a negative factor as discussed

// --- NEW MODULE D PINS ---
#define PIN_BTN_VOID 10
#define PIN_BTN_CHECKOUT 11
#define PIN_BUZZER 12
#define PIN_LED 13

// Global Data Structures
struct Item {
  bool found;
  String sku;
  String name;
  float price;
  float weight;
};

// Include module headers
#include "Barcode.h"
#include "Inventory.h"
#include "Display.h"
#include "Scale.h"
#include "SystemIO.h" // NEW

#endif