#include "Barcode.h"

BarcodeScanner::BarcodeScanner() {
  currentBuffer = "";
  lastScannedCode = "";
  newDataAvailable = false;
}

void BarcodeScanner::beginScanner() {
  this->begin();
  // If your scanner outputs strange characters, you may need to add:
  // this->setHIDLocal(HID_LOCAL_Japan_Katakana); 
}

void BarcodeScanner::onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) {
  // If the character is a standard printable character, add it to our string
  if (' ' <= ascii && ascii <= '~') {
    currentBuffer += (char)ascii;
  } 
  // Detect the end of the barcode transmission
  else if (ascii == '\r' || ascii == '\n') {
    if (currentBuffer.length() > 0) {
      lastScannedCode = currentBuffer;
      newDataAvailable = true;
      currentBuffer = ""; // Clear the buffer for the next scan
    }
  }
}

bool BarcodeScanner::hasNewData() {
  return newDataAvailable;
}

String BarcodeScanner::getScannedCode() {
  // Clear the flag once the data is read by the main system
  newDataAvailable = false; 
  return lastScannedCode;
}