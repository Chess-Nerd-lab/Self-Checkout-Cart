#include "modules.h"

BarcodeScanner scanner;
InventoryDB inventory;
SystemDisplay display;
SecurityScale bagScale;
SystemIO sysIO;

enum SystemState {
  STATE_STARTUP_CHECK,
  STATE_IDLE,
  STATE_CHECKOUT_MODE,
  STATE_PROCESSING_SCAN,
  STATE_WAITING_FOR_SCALE,
  STATE_REMOVAL_REQUESTED,
  STATE_WAITING_FOR_REMOVAL,
  STATE_VIEW_CART,
  STATE_TAMPER_ALARM,
  STATE_REMOVE_MODE,  // NEW: Dedicated removal state
  STATE_ADMIN_LOCK,    // NEW: Strict lockdown state
  STATE_AWAITING_PAYMENT
};

SystemState currentState = STATE_STARTUP_CHECK;
std::vector<Item> cart;

float cartTotal = 0.0;
float stableCartWeight = 0.0; 
Item currentItem; 
unsigned long stateTimer = 0;
int cartViewIndex = 0;

// --- NEW VARIABLES FOR BUTTON 10 & ADMIN ---
const String ADMIN_BARCODE = "ITEM_00006";
const int BTN_10_PIN = 10;
unsigned long btn10PressTime = 0;
bool btn10Pressed = false;
bool btn10Handled = false;
bool isCheckoutRemoval = false;

void setup() {
  Serial.begin(115200);

  pinMode(BTN_10_PIN, INPUT_PULLUP);

  delay(1000); 
  
  display.begin();
  sysIO.begin(); 
  
  display.printBootStep(0, "System Initialising.");
  inventory.begin(); 
  
  display.printBootStep(1, "Starting Scale...");
  bagScale.begin(HX711_DOUT_PIN, HX711_SCK_PIN, SCALE_CAL_FACTOR);
  
  display.printBootStep(2, "Starting Scanner...");
  scanner.beginScanner();
  
  delay(1000);
}

void loop() {
  scanner.task();
  sysIO.update(); // Keep button states updated

  switch (currentState) {

    // --- 9. FINAL PAYMENT VERIFICATION ---
    case STATE_AWAITING_PAYMENT: {
      if (scanner.hasNewData()) {
        String scannedCode = scanner.getScannedCode();
        scannedCode.trim();
        
        if (scannedCode == ADMIN_BARCODE) {
          sysIO.successSignal();
          display.showInstruction("Checkout Complete!  ");
          delay(3000);
          
          // Full Reset for the next customer
          cart.clear(); 
          cartTotal = 0.0; 
          stableCartWeight = bagScale.getCurrentWeight(); // Scale should be empty (~0g)
          bagScale.tareScale(); 
          
          display.setupActiveUI();
          currentState = STATE_IDLE;
        } else {
          sysIO.errorSignal();
          display.showInstruction("Admin Code Needed!  ");
          delay(2000);
          display.showInstruction("Make Payment        ");
        }
      }
      break;
    }
    
    // --- 5. STARTUP CHECK ---
    case STATE_STARTUP_CHECK: {
      long rawReading = bagScale.getRawValue();
      if (rawReading != 0) { // Ensure HX711 is responding
        if (abs(rawReading - SCALE_EMPTY_RAW) > 10000) { // Adjust threshold as needed
          display.showInstruction("CLEAR SCALE TO BOOT!");
          sysIO.alarmOn();
        } else {
          sysIO.alarmOff();
          bagScale.tareScale();
          display.setupActiveUI();
          currentState = STATE_IDLE;
        }
      }
      break;
    }

    // --- 1. NORMAL IDLE ---
    case STATE_IDLE: {
      float currentWt = bagScale.getCurrentWeight();
      float difference = abs(currentWt - stableCartWeight);

      // 6. IDLE TAMPERING ALARM
      if (difference > bagScale.getTolerance(stableCartWeight)) {
        sysIO.alarmOn();
        display.showInstruction("TAMPER DETECTED!    ");
        currentState = STATE_TAMPER_ALARM;
        break;
      }
      else if (difference > 0.2 && difference <= 3.0) {
        stableCartWeight = currentWt;
      }

      // --- CUSTOM BUTTON 10 LOGIC (Short = View, Long = Remove) ---
      bool currentBtnState = (digitalRead(BTN_10_PIN) == LOW); // True if pressed

      if (currentBtnState && !btn10Pressed) {
        btn10Pressed = true;
        btn10PressTime = millis();
        btn10Handled = false;
      }
      
      // LONG PRESS (Hold for 1.5 seconds)
      if (currentBtnState && btn10Pressed && !btn10Handled) {
        if (millis() - btn10PressTime > 1500) { 
          btn10Handled = true;
          display.showMode("== REMOVE MODE ==");
          display.showInstruction("Scan to REMOVE item");
          sysIO.successSignal();
          currentState = STATE_REMOVE_MODE;
        }
      }
      
      // SHORT PRESS (Release before 1.5 seconds)
      if (!currentBtnState && btn10Pressed) {
        if (!btn10Handled && (millis() - btn10PressTime > 50)) { 
          if (cart.size() > 0) {
            cartViewIndex = 0;
            stateTimer = millis();
            display.showCartItem(cart[0], 0, cart.size());
            currentState = STATE_VIEW_CART;
          } else {
             display.showInstruction("Cart is empty!      ");
             delay(1500);
             display.setupActiveUI();
          }
        }
        btn10Pressed = false;
      }

      // --- TRIGGER CHECKOUT MODE (Button 11) ---
      else if (sysIO.isCheckoutSingleClicked() || sysIO.isCheckoutDoubleClicked()) {
        if (cart.size() > 0) {
          display.showMode("== CHECKOUT MODE ==");
          display.showInstruction("Remove items        ");
          sysIO.successSignal();
          currentState = STATE_CHECKOUT_MODE;
        } else {
          display.showInstruction("Cart is empty!      ");
          delay(1500);
          display.setupActiveUI();
        }
      }

      // Check Scanner
      else if (scanner.hasNewData()) {
        String scannedCode = scanner.getScannedCode();
        scannedCode.trim(); 
        currentItem = inventory.getItemByBarcode(scannedCode);
        
        if (currentItem.found) {
          display.showInstruction("Place item on scale!");
          currentState = STATE_WAITING_FOR_SCALE;
        } else {
          sysIO.errorSignal();
          display.showError();
          delay(2000); display.setupActiveUI(); display.updateCart(currentItem, cartTotal);
        }
      }
      break;
    }

    // --- 2. DYNAMIC ADDING ALARM ---
    case STATE_WAITING_FOR_SCALE: {
      float reading = bagScale.getCurrentWeight();
      float targetWeight = stableCartWeight + currentItem.weight;
      float tol = bagScale.getTolerance(currentItem.weight);

      // ---TEMPORARY TEST---
      //float tol = 50.0; // Forced to 50 grams for testing!

      /*Serial.print("--- UNIT CHECK ---");
      Serial.print("Database says item is: "); Serial.print(currentItem.weight); Serial.println(" units");
      Serial.print("Physical Scale reads:  "); Serial.print(reading); Serial.println(" units");
      Serial.print("Calculated Target is:  "); Serial.println(targetWeight);
      Serial.println("------------------");*/

      if (abs(reading - targetWeight) <= tol) {
        // Item placed correctly
        sysIO.alarmOff(); sysIO.successSignal();
        stableCartWeight = reading;
        cartTotal += currentItem.price;
        cart.push_back(currentItem); // Add to vector
        display.updateCart(currentItem, cartTotal);
        currentState = STATE_IDLE;
      } 
      else if (abs(reading - stableCartWeight) > tol) {
        // Incorrect weight added
        sysIO.alarmOn();
        display.showInstruction("Weight Mismatch!    ");
      } 
      else {
        // Waiting for placement (weight hasn't changed yet)
        sysIO.alarmOff();
        display.showInstruction("Place item on scale!");
      }
      break;
    }

    // --- 3. CHECKOUT / REMOVAL MODE ---
    case STATE_CHECKOUT_MODE: {
      // 1. Check if everything has been removed
      if (cart.size() == 0) {
        display.showMode("== PAYMENT ==");
        display.showInstruction("Make Payment        ");
        sysIO.successSignal();
        currentState = STATE_AWAITING_PAYMENT;
        break;
      }

      // 2. Only allow scanning to REMOVE items
      if (scanner.hasNewData()) {
        String scannedCode = scanner.getScannedCode();
        scannedCode.trim();
        
        // Check if item is in our cart vector
        int foundIndex = -1;
        for(size_t i=0; i < cart.size(); i++) {
          if (cart[i].sku == scannedCode) { 
            foundIndex = i;
            break; 
          }
        }

        if (foundIndex != -1) {
          currentItem = cart[foundIndex];
          cart.erase(cart.begin() + foundIndex);
          display.showInstruction("Remove from scale!  ");
          
          isCheckoutRemoval = true; // FLAG IT: Tell the system we are in checkout!
          currentState = STATE_WAITING_FOR_REMOVAL;
        } else {
          sysIO.errorSignal();
          display.showInstruction("Not in cart!        ");
          delay(2000); 
          display.showInstruction("Remove items        ");
        }
      }
      break;
    }

    // --- 2. DYNAMIC REMOVAL ALARM ---
    //NEW CODE
    case STATE_WAITING_FOR_REMOVAL: {
      float reading = bagScale.getCurrentWeight();
      float targetWeight = stableCartWeight - currentItem.weight;
      float tol = bagScale.getTolerance(currentItem.weight);

      if (abs(reading - targetWeight) <= tol) {
        sysIO.alarmOff(); 
        sysIO.successSignal();
        stableCartWeight = reading;

        // ONLY subtract the price if we are in normal shopping mode!
        // If we are in checkout mode, keep the total the same so they can pay.
        if (!isCheckoutRemoval) {
          cartTotal -= currentItem.price;
          if (cartTotal < 0) cartTotal = 0;
        }
        
        display.showInstruction("Item Removed Success");
        delay(2000);
        display.setupActiveUI();
        display.updateCart(currentItem, cartTotal);

        // --- NEW ROUTING LOGIC ---
        if (isCheckoutRemoval) {
          currentState = STATE_CHECKOUT_MODE; // Send back to checkout loop!
          display.showInstruction("Remove items        ");
        } else {
          display.setupActiveUI();
          display.updateCart(currentItem, cartTotal);
          currentState = STATE_IDLE; // Normal mid-shopping behavior
        }
      }
       
      else if (abs(reading - stableCartWeight) > tol) {
        // SCAM DETECTED: Weight didn't match the item scanned!
        sysIO.alarmOn();
        display.showInstruction("Removal Mismatch!   ");
        currentState = STATE_ADMIN_LOCK; // Send to strict lockdown
      } 
      else {
        sysIO.alarmOff();
        display.showInstruction("Remove from scale!  ");
      }
      break;
    }
    
    // --- 7. DEDICATED REMOVE MODE ---
    case STATE_REMOVE_MODE: {
      if (scanner.hasNewData()) {
        String scannedCode = scanner.getScannedCode();
        scannedCode.trim();
        
        // Search the cart vector for the scanned item
        int foundIndex = -1;
        for(size_t i = 0; i < cart.size(); i++) {
          if (cart[i].sku == scannedCode) { 
            foundIndex = i;
            break; 
          }
        }

        if (foundIndex != -1) {
          
          // Item found in cart!Temporarily erase it and wait for physical scale removal
          currentItem = cart[foundIndex];
          cart.erase(cart.begin() + foundIndex); 
          display.showInstruction("Remove from scale!  ");

          isCheckoutRemoval = false; // FLAG IT: Normal mid-shopping removal--- NEW CODE
          currentState = STATE_WAITING_FOR_REMOVAL;
        } else {
          // SCAM DETECTED: Trying to remove an item that was never added!
          sysIO.errorSignal();
          sysIO.alarmOn();
          display.showInstruction("ALARM! Not in Cart! ");
          currentState = STATE_ADMIN_LOCK; // Send to strict lockdown
        }
      }
      break;
    }

    // --- 8. STRICT ADMIN LOCKDOWN ---
    case STATE_ADMIN_LOCK: {
      // System is completely locked. Continually ensures alarm is on.
      sysIO.alarmOn(); 
      
      // Flash instruction occasionally so they know what to do
      if (millis() % 2000 < 1000) {
        display.showInstruction("ALARM! Admin Reqd.  ");
      } else {
        display.showInstruction("Scan Admin Barcode  ");
      }

      if (scanner.hasNewData()) {
        String scannedCode = scanner.getScannedCode();
        scannedCode.trim();

        if (scannedCode == ADMIN_BARCODE) {
          sysIO.alarmOff();
          sysIO.successSignal();
          display.showInstruction("Admin Override OK   ");
          delay(2000);
          
          // Re-sync the software weight with the physical weight so it doesn't instantly alarm again
          stableCartWeight = bagScale.getCurrentWeight();
          
          display.setupActiveUI();
          display.updateCart(currentItem, cartTotal); 
          currentState = STATE_IDLE;
        } else {
          sysIO.errorSignal();
          display.showInstruction("Invalid Admin Code  ");
          delay(1500);
        }
      }
      break;
    }

    // --- 4. CART VIEWER AUTO-SCROLL ---
    case STATE_VIEW_CART: {
      if (millis() - stateTimer > 1500) { // 1.5 seconds per item
        stateTimer = millis();
        cartViewIndex++;
        
        if (cartViewIndex >= cart.size()) {
          display.setupActiveUI();
          display.updateCart(currentItem, cartTotal);
          currentState = STATE_IDLE;
        } else {
          display.showCartItem(cart[cartViewIndex], cartViewIndex, cart.size());
        }
      }
      break;
    }

    // --- 6. SECURITY LOCK ---
    case STATE_TAMPER_ALARM: {
      float currentWt = bagScale.getCurrentWeight();
      if (abs(currentWt - stableCartWeight) <= bagScale.getTolerance(stableCartWeight)) {
        sysIO.alarmOff();
        display.setupActiveUI();
        display.updateCart(currentItem, cartTotal);
        currentState = STATE_IDLE;
      }
      break;
    }
  }
}