#include "EspUsbHost.h"

class MyEspUsbHost : public EspUsbHost {
  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) {
    if (' ' <= ascii && ascii <= '~') {
      Serial.printf("%c", ascii);
    } else if (ascii == '\r') {
      Serial.println();
    }
  };
};

MyEspUsbHost usbHost;

void setup() {
  Serial.begin(115200);
  delay(2000); // Give the serial monitor time to open
  
  Serial.println("ESP32-S3 USB Host Started!"); // ADD THIS LINE

  usbHost.begin();
  // usbHost.setHIDLocal(HID_LOCAL_Japan_Katakana); // Or try removing this line if your scanner is standard US layout
}

void loop() {
  usbHost.task();
}
