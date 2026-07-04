#ifndef MODULES_H
#define MODULES_H

#include <Arduino.h>

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

// Future includes: #include "Scale.h", #include "Display.h"

#endif