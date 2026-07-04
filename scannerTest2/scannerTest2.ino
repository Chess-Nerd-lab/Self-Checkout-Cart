#include "EspUsbHost.h"

class MyEspUsbHost : public EspUsbHost {
  String barcodeData = ""; // Buffer to hold the scanned characters

  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) {
    // If the character is a standard printable character, add it to our string
    if (' ' <= ascii && ascii <= '~') {
      barcodeData += (char)ascii;
    } 
    // Most scanners send a Carriage Return (\r) or Newline (\n) at the end of a scan
    else if (ascii == '\r' || ascii == '\n') {
      if (barcodeData.length() > 0) {
        Serial.print("✅ Scanned Result: ");
        Serial.println(barcodeData);
        barcodeData = ""; // Clear the buffer for the next scan
      }
    }
  };
};

MyEspUsbHost usbHost;

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial monitor a moment
  
  Serial.println("ESP32-S3 Scanner Ready. Awaiting scan...");
  
  usbHost.begin();
  // Note: I removed the setHIDLocal line. If your scanner types weird 
  // symbols instead of numbers, you might need to put it back.
}

void loop() {
  usbHost.task();
}