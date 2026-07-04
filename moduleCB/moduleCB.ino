#include "modules.h"

// Instantiate the modules
BarcodeScanner scanner;
InventoryDB inventory;

void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.println("=====================================");
  Serial.println("  Self-Checkout System Initializing  ");
  Serial.println("=====================================");
  
  Serial.print("Starting Inventory Database... ");
  inventory.begin();

  Serial.print("Starting Barcode Module... ");
  scanner.beginScanner();
  Serial.println("Done.");
  
  Serial.println("\n🛒 System Ready. Awaiting items...");
}

void loop() {
  // 1. Maintain USB Host Background Tasks
  scanner.task();

  // 2. Process New Scans
  if (scanner.hasNewData()) {
    String scannedCode = scanner.getScannedCode();
    
    // Remove hidden newline/carriage return characters
    scannedCode.trim(); 

    Serial.println("\n-------------------------------------");
    Serial.print("🔍 Scanned SKU: ");
    Serial.println(scannedCode);
    
    // 3. Query the Database
    Item scannedItem = inventory.getItemByBarcode(scannedCode);

    if (scannedItem.found) {
      Serial.println("✅ Item Found in Database:");
      Serial.print("   Name:   "); Serial.println(scannedItem.name);
      Serial.print("   Price:  "); Serial.println(scannedItem.price, 2);
      Serial.print("   Weight: "); Serial.print(scannedItem.weight); Serial.println("g");
    } else {
      Serial.println("❌ Item not recognized. Please call for assistance.");
    }
    Serial.println("-------------------------------------");
  }

  // Keep the slight delay to allow USB interrupts to process smoothly
  delay(1); 
}