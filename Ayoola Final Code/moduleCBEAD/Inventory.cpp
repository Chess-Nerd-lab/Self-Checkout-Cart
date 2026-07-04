#include "Inventory.h"

InventoryDB::InventoryDB() {}

bool InventoryDB::begin() {
  if (!LittleFS.begin(true)) { 
    Serial.println("❌ Error: LittleFS Mount Failed!");
    return false;
  }

  if (!LittleFS.exists("/items.json")) {
    Serial.println("❌ Error: /items.json not found in LittleFS!");
    return false;
  }

  Serial.println("✅ LittleFS Mounted & items.json found.");
  return true;
}

Item InventoryDB::getItemByBarcode(String barcode) {
  // Default empty item to return if not found
  Item result = {false, "", "Unknown Item", 0.0, 0.0};

  File file = LittleFS.open("/items.json", "r");
  if (!file) {
    Serial.println("❌ Error: Failed to open items.json for reading");
    return result;
  }

  // Allocate memory for the JSON document.
  JsonDocument doc; 
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("❌ JSON Parse Error: ");
    Serial.println(error.c_str());
    return result;
  }

  // Extract the "items" array from the JSON
  JsonArray itemsArray = doc["items"];

  // Iterate through the array to find the matching SKU
  for (JsonObject item : itemsArray) {
    if (item["sku"].as<String>() == barcode) {
      result.found = true;
      result.sku = item["sku"].as<String>();
      result.name = item["name"].as<String>();
      result.price = item["price"].as<float>();
      result.weight = item["weight"].as<float>();
      break; // Exit the loop once we find the item
    }
  }

  return result;
}