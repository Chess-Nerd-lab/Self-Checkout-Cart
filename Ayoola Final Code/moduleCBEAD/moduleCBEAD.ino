#include "modules.h"

// Instantiate the modules
BarcodeScanner scanner;
InventoryDB inventory;
SystemDisplay display;
SecurityScale bagScale;
SystemIO sysIO; // NEW

// Expanded System States
enum SystemState {
  STATE_IDLE,
  STATE_PROCESSING_SCAN,
  STATE_WAITING_FOR_SCALE,
  STATE_ERROR,
  STATE_VOID_REQUESTED,
  STATE_WAITING_FOR_REMOVAL,
  STATE_CHECKOUT_CONFIRM,
  STATE_CHECKOUT_DONE
};

SystemState currentState = STATE_IDLE;
float cartTotal = 0.0;
Item currentItem; 
unsigned long stateTimer = 0;

void setup() {
  Serial.begin(115200);
  delay(1000); 
  
  display.begin();
  sysIO.begin(); // Initialize Module D hardware
  
  display.printBootStep(0, "System Initialising.");
  delay(500); 
  
  display.printBootStep(1, "Starting Database...");
  inventory.begin(); 
  
  display.printBootStep(2, "Starting Scale...");
  bagScale.begin(HX711_DOUT_PIN, HX711_SCK_PIN, SCALE_CAL_FACTOR);
  
  display.printBootStep(3, "Starting Scanner...");
  scanner.beginScanner();
  
  delay(1000);
  display.setupActiveUI();
}

void loop() {
  // ALWAYS maintain USB Host Background Tasks
  scanner.task();

  // State Machine Logic
  switch (currentState) {
    
    case STATE_IDLE:
      // Check for user inputs
      if (sysIO.isVoidPressed()) {
        display.showInstruction("Scan item to VOID...");
        currentState = STATE_VOID_REQUESTED;
      } 
      else if (sysIO.isCheckoutPressed()) {
        display.showInstruction("Press again to PAY");
        stateTimer = millis();
        currentState = STATE_CHECKOUT_CONFIRM;
      }
      // Check for normal scanning
      else if (scanner.hasNewData()) {
        String scannedCode = scanner.getScannedCode();
        scannedCode.trim(); 
        currentItem = inventory.getItemByBarcode(scannedCode);
        currentState = STATE_PROCESSING_SCAN;
      }
      break;

    case STATE_PROCESSING_SCAN:
      if (currentItem.found) {
        display.showInstruction("Place item on scale!");
        currentState = STATE_WAITING_FOR_SCALE;
      } else {
        sysIO.errorSignal();
        display.showError();
        stateTimer = millis(); 
        currentState = STATE_ERROR;
      }
      break;

    case STATE_WAITING_FOR_SCALE:
      if (bagScale.waitForItem(currentItem.weight)) {
        sysIO.successSignal();
        cartTotal += currentItem.price;
        display.updateCart(currentItem, cartTotal);
        currentState = STATE_IDLE; 
      } else {
        sysIO.errorSignal();
        display.showInstruction("Weight Mismatch!    ");
        stateTimer = millis();
        currentState = STATE_ERROR;
      }
      break;

    // --- NEW: ITEM REMOVAL SEQUENCE ---
    case STATE_VOID_REQUESTED:
      if (scanner.hasNewData()) {
        String scannedCode = scanner.getScannedCode();
        scannedCode.trim();
        currentItem = inventory.getItemByBarcode(scannedCode);
        
        if (currentItem.found) {
          display.showInstruction("Remove from scale!  ");
          currentState = STATE_WAITING_FOR_REMOVAL;
        } else {
          sysIO.errorSignal();
          display.showError();
          stateTimer = millis();
          currentState = STATE_ERROR;
        }
      }
      // Allow the user to cancel the void process by pressing the void button again
      if (sysIO.isVoidPressed()) {
        display.updateCart(currentItem, cartTotal); // Revert to idle display
        currentState = STATE_IDLE;
      }
      break;

    case STATE_WAITING_FOR_REMOVAL:
      // We pass the NEGATIVE weight. The scale waits for the weight to drop!
      if (bagScale.waitForItem(-currentItem.weight)) {
        sysIO.successSignal();
        cartTotal -= currentItem.price;
        if (cartTotal < 0) cartTotal = 0; // Prevent negative totals
        display.updateCart(currentItem, cartTotal);
        currentState = STATE_IDLE;
      } else {
        sysIO.errorSignal();
        display.showInstruction("Removal Mismatch!   ");
        stateTimer = millis();
        currentState = STATE_ERROR;
      }
      break;

    // --- NEW: CHECKOUT SEQUENCE ---
    case STATE_CHECKOUT_CONFIRM:
      if (sysIO.isCheckoutPressed()) {
        currentState = STATE_CHECKOUT_DONE;
      }
      // Timeout if they don't press it a second time within 5 seconds
      if (millis() - stateTimer > 5000) {
        display.updateCart(currentItem, cartTotal); // Revert UI
        currentState = STATE_IDLE;
      }
      break;

    case STATE_CHECKOUT_DONE:
      display.showInstruction("Payment Approved.   ");
      sysIO.successSignal();
      delay(3000); // Give user time to see the success message
      
      // Reset the system for the next customer
      cartTotal = 0.0;
      bagScale.tareScale(); // Re-zero the scale 
      currentItem.name = "Welcome!";
      currentItem.price = 0.0;
      display.setupActiveUI();
      display.updateCart(currentItem, cartTotal);
      
      currentState = STATE_IDLE;
      break;

    // --- ERROR HANDLING ---
    case STATE_ERROR:
      // Hold the error screen for 3 seconds, then return to normal
      if (millis() - stateTimer > 3000) {
        display.setupActiveUI(); 
        if (cartTotal > 0 || currentItem.found) {
          display.updateCart(currentItem, cartTotal); 
        }
        currentState = STATE_IDLE;
      }
      break;
  }

  delay(1); // FreeRTOS breather
}