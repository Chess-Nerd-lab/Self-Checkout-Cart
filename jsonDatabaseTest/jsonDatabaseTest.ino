#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Configuration ---
// For ESP32-S3, default I2C is often GPIO 8 (SDA) and GPIO 9 (SCL)
#define I2C_SDA 8
#define I2C_SCL 9
#define LCD_ADDR 0x27 // Common address for I2C LCDs, change to 0x3F if 0x27 fails
#define LCD_COLS 20
#define LCD_ROWS 4

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Struct to hold our retrieved data
struct Product {
  String name;
  float price;
  int weight;
  bool found = false;
};

// --- Function: Search Database ---
Product getProductBySKU(String scannedSKU) {
  Product p;
  
  // 1. Open the file
  File file = LittleFS.open("/items.json", "r");
  if (!file) {
    Serial.println("❌ Failed to open items.json");
    return p;
  }

  // 2. Parse the JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("❌ JSON Parsing failed: ");
    Serial.println(error.c_str());
    file.close();
    return p;
  }

  // 3. Search the items array
  JsonArray items = doc["items"];
  for (JsonObject item : items) {
    if (item["sku"] == scannedSKU) {
      p.name = item["name"].as<String>();
      p.price = item["price"].as<float>();
      p.weight = item["weight"].as<int>();
      p.found = true;
      break; 
    }
  }

  file.close();
  return p;
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for Serial Monitor

  // Initialize I2C and LCD
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("❌ LittleFS Mount Failed!");
    lcd.setCursor(0, 1);
    lcd.print("FS Error!");
    return;
  }
  Serial.println("✅ LittleFS Mounted.");

  // --- Test Search ---
  String testSKU = "ITEM-00001";
  Serial.print("Searching for: ");
  Serial.println(testSKU);

  Product result = getProductBySKU(testSKU);

  if (result.found) {
    Serial.println("✅ Item Found!");
    Serial.println("Name: " + result.name);
    Serial.print("Price: $"); Serial.println(result.price);
    Serial.print("Expected Weight: "); Serial.print(result.weight); Serial.println("g");

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(result.name);
    lcd.setCursor(0, 1);
    lcd.print("Price: $");
    lcd.print(result.price);
  } else {
    Serial.println("⚠️ Item NOT found in database.");
    lcd.clear();
    lcd.print("Not Found!");
  }
}

void loop() {
  // Nothing here for the test
}