#ifndef BARCODE_H
#define BARCODE_H

#include <Arduino.h>
#include "EspUsbHost.h"

class BarcodeScanner : public EspUsbHost {
  private:
    String currentBuffer;
    String lastScannedCode;
    bool newDataAvailable;

  public:
    // Constructor
    BarcodeScanner();

    // Initialization routine
    void beginScanner();

    // The overridden callback from EspUsbHost
    void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) override;

    // Functions for the main loop to interact with
    bool hasNewData();
    String getScannedCode();
};

#endif