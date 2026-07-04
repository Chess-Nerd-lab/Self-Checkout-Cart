#include "modules.h"

// Instantiate the barcode scanner module
BarcodeScanner scanner;

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow time for the serial monitor to connect

  Serial.println("=====================================");
  Serial.println("  Self-Checkout System Initializing  ");
  Serial.println("=====================================");
  
  Serial.print("Starting Barcode Module... ");
  scanner.beginScanner();
  Serial.println("Done.");
  
  Serial.println("\nSystem Ready. Awaiting items...");
}

void loop() {
  // 1. Maintain USB Host Background Tasks (Must run continuously)
  scanner.task();

  // 2. Process New Scans
  if (scanner.hasNewData()) {
    String scannedItemCode = scanner.getScannedCode();
    
    Serial.print("✅ Item Scanned: ");
    Serial.println(scannedItemCode);
    
    // Future integration points:
    // - Look up scannedItemCode in an inventory database array
    // - Display the item name and price on an LCD
    // - Require the user to place the item on the load cells to verify weight
  }

  // 3. Other system tasks will go here (e.g., scale.update(), button.check())
}