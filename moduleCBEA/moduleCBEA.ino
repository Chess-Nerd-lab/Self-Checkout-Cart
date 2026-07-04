#include "modules.h"

// Instantiate the modules
BarcodeScanner scanner;
InventoryDB inventory;
SystemDisplay display;
SecurityScale bagScale; // NEW

// Global tracking variable
float cartTotal = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000); 
  
  display.begin();
  
  display.printBootStep(0, "System Initialising.");
  delay(1000); 
  
  display.printBootStep(1, "Starting Database...");
  inventory.begin();
  
  display.printBootStep(2, "Starting Scale...");
  // Initialize scale with your magic calibration factor
  bagScale.begin(HX711_DOUT_PIN, HX711_SCK_PIN, SCALE_CAL_FACTOR);
  
  display.printBootStep(3, "Starting Scanner...");
  scanner.beginScanner();
  
  delay(1000);
  display.setupActiveUI();
}

void loop() {
  // 1. Maintain USB Host Background Tasks
  scanner.task();

  // 2. Process New Scans
  if (scanner.hasNewData()) {
    String scannedCode = scanner.getScannedCode();
    scannedCode.trim(); 
    
    Item scannedItem = inventory.getItemByBarcode(scannedCode);

    if (scannedItem.found) {
      Serial.println("Item Scanned. Waiting for placement...");
      
      // Update display to ask the user to place the item
      display.showInstruction("Place item on scale!");
      
      // 3. Halt and verify the weight
      bool weightVerified = bagScale.waitForItem(scannedItem.weight);

      if (weightVerified) {
        Serial.println("Weight verified. Added to cart.");
        cartTotal += scannedItem.price;
        display.updateCart(scannedItem, cartTotal);
      } else {
        Serial.println("Error: Weight Mismatch or Timeout.");
        display.showInstruction("Weight Mismatch!    ");
        // In a real system, you might require an admin override here
        // For now, we will just delay so the user sees the error, then revert the UI
        delay(3000); 
        display.updateCart(scannedItem, cartTotal); // Revert to showing the item, but price isn't added
      }
      
    } else {
      display.showError();
      Serial.println("Error: Unknown SKU - " + scannedCode);
      delay(3000);
      display.setupActiveUI(); // Reset UI
    }
  }

  delay(1); 
}