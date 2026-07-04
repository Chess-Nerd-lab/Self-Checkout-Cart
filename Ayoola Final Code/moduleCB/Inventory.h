#ifndef INVENTORY_H
#define INVENTORY_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "modules.h"

class InventoryDB {
  public:
    InventoryDB();
    
    // Mounts LittleFS and checks for the JSON file
    bool begin();
    
    // Searches the JSON database for a specific barcode
    Item getItemByBarcode(String barcode);
};

#endif