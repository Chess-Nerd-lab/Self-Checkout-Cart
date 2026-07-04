#include "Display.h"

SystemDisplay::SystemDisplay() : lcd(0x27, 20, 4) {}

void SystemDisplay::begin() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void SystemDisplay::printBootStep(uint8_t row, String message) {
  lcd.setCursor(0, row);
  // Pad with spaces to clear any existing artifacts on that line
  while(message.length() < 20) message += " ";
  lcd.print(message);
}

void SystemDisplay::setupActiveUI() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Last Scanned Item:");
  lcd.setCursor(0, 3);
  lcd.print("TOTAL: 0.00         "); // Default starting total
}

void SystemDisplay::updateCart(Item item, float currentTotal) {
  // 1. Update the Item Name (Row 1)
  String nameLine = item.name;
  while(nameLine.length() < 20) nameLine += " ";
  lcd.setCursor(0, 1);
  lcd.print(nameLine);

  // 2. Update the Price (Row 2)
  String priceLine = "Price: " + String(item.price, 2);
  while(priceLine.length() < 20) priceLine += " ";
  lcd.setCursor(0, 2);
  lcd.print(priceLine);

  // 3. Update the Running Total (Row 3)
  // We right-align the total amount for a cleaner cash-register look
  String totalLine = "TOTAL: " + String(currentTotal, 2);
  String paddedTotal = "";
  for(int i = 0; i < (20 - totalLine.length()); i++) {
    paddedTotal += " ";
  }
  paddedTotal += totalLine;
  
  lcd.setCursor(0, 3);
  lcd.print(paddedTotal);
}

void SystemDisplay::showError() {
  lcd.setCursor(0, 1);
  lcd.print("Item Not Found!     ");
  lcd.setCursor(0, 2);
  lcd.print("Seek assistance.    ");
}