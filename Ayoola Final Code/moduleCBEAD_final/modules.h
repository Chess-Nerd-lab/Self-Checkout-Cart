#ifndef MODULES_H
#define MODULES_H

#include <Arduino.h>
#include <vector>

// Hardware Pin Definitions
#define HX711_DOUT_PIN 4
#define HX711_SCK_PIN 5
#define SCALE_CAL_FACTOR 1000.0 
#define SCALE_EMPTY_RAW 232000 // CHANGE THIS: Read the raw value from calibration sketch

// Module D Pins
#define PIN_BTN_VIEW 10      // Formerly Void, now View Cart
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
#include "SystemIO.h"

#endif