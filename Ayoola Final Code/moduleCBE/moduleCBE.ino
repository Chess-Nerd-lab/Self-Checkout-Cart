#include "modules.h"

// Instantiate the modules
BarcodeScanner scanner;
InventoryDB inventory;
SystemDisplay display;

// Global tracking variable
float cartTotal = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000); 
  
  // 1. Initialize the display first so we can see the boot sequence
  display.begin();
  
  // Row 0
  display.printBootStep(0, "System Initialising.");
  delay(1000); // Requested 1-second delay
  
  // Row 1
  display.printBootStep(1, "Starting Database...");
  inventory.begin();
  
  // Row 2
  display.printBootStep(2, "Starting Scanner...");
  scanner.beginScanner();
  
  // Row 3
  display.printBootStep(3, "System Ready.");
  delay(1500); // Hold the ready screen briefly so the user can read it
  
  // Clear the boot sequence and draw the active UI template
  display.setupActiveUI();
}

void loop() {
  // Maintain USB Host Background Tasks
  scanner.task();

  // Process New Scans
  if (scanner.hasNewData()) {
    String scannedCode = scanner.getScannedCode();
    scannedCode.trim(); 
    
    Item scannedItem = inventory.getItemByBarcode(scannedCode);

    if (scannedItem.found) {
      // Add to the running total and update the physical screen
      cartTotal += scannedItem.price;
      display.updateCart(scannedItem, cartTotal);
      
      // Keep Serial logging for debugging
      Serial.println("Scanned: " + scannedItem.name);
    } else {
      display.showError();
      Serial.println("Error: Unknown SKU - " + scannedCode);
    }
  }

  // Keep the slight delay for FreeRTOS background tasks
  delay(1); 
}