#ifndef MODULES_H
#define MODULES_H

#include <Arduino.h>

// Hardware Pin Definitions
#define HX711_DOUT_PIN 4
#define HX711_SCK_PIN 5
#define SCALE_CAL_FACTOR 200.0 

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
#include "Scale.h" // NEW

#endif