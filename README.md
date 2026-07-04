Smart Self-Checkout Shopping Cart System
An embedded retail solution designed to decentralize traditional supermarket checkout lanes. Powered by an ESP32-S3 microcontroller, this system transforms a standard shopping cart into a mobile Point-of-Sale (POS) terminal with real-time, hardware-enforced theft prevention.

Consumers can scan items as they shop, view their running total on an integrated display, and seamlessly check out—all while a weight scale continuously verifies that the physical items in the cart match the software database.

Key Features
Real-Time Barcode Scanning: Fast and reliable item additions using a TTL/UART hardware barcode scanner.

Hardware Theft Protection: Strict weight verification utilizing an HX711 load cell. If an item is scanned but not placed, or placed but not scanned, the system flags a "Weight Mismatch" or "Tamper" alarm.

Non-Blocking Architecture: Built on a robust Finite State Machine (FSM) ensuring the scanner, scale, display, and buttons run concurrently without system freezes.

Dual-Action Cart Management:
View Cart: A short press of a (yellow) button auto-scrolls through the items currently in the cart.

Remove Mode: A long press (yellow button) safely removes items, actively verifying that the correct weight is subtracted from the cart.

Secure Checkout Flow: A checkout mode that locks out new item additions and forces the user to empty the cart. The system remains locked until a store administrator scans a verification barcode.

Hardware Requirements
Microcontroller: ESP32-S3 Development Board (Requires native USB/UART support)

Scale: Load Cell + HX711 Amplifier Module

Scanner: 2D Barcode Scanner

Display: I2C LCD Display (20x4)

Inputs/Outputs:

Push Button 1 (Pin 10): View Cart / Remove Item

Push Button 2 (Pin 11): Enter Checkout Mode

Active Buzzer: Security Alarms and Success Chimes

Software Architecture

System States:
STATE_STARTUP_CHECK: Ensures the scale is empty (0g) before booting.

STATE_IDLE: The default shopping phase. Monitors for passive tampering.

STATE_WAITING_FOR_SCALE: Awaits weight confirmation after an item is scanned.

STATE_REMOVE_MODE: Specialized mode allowing users to scan items they wish to remove.

STATE_WAITING_FOR_REMOVAL: Validates that the exact weight of the removed item is taken out of the cart.

STATE_VIEW_CART: Temporarily scrolls the LCD to show the user's current cart list.

STATE_CHECKOUT_MODE: Locks the cart for unbagging. Only removals are permitted.

STATE_TAMPER_ALARM: Triggered by un-scanned weight fluctuations.

STATE_ADMIN_LOCK: Strict lockdown state triggered by a mismatched removal or theft attempt. Requires the systemresetpassword12345 barcode.

STATE_AWAITING_PAYMENT: Final lock requiring staff verification to complete the transaction.

💻 Installation & Setup
1. Arduino IDE Configuration
Install the ESP32 Board Package in the Arduino IDE.

Go to Tools > Board and select ESP32S3 Dev Module.

Under Tools, ensure USB CDC On Boot is set to Enabled.

2. Required Libraries
Ensure the following libraries are installed via the Arduino Library Manager:

ArduinoJSON by Benoit Blanchon (version 7.4.3)

HX711 by Bogdan Necula (for the load cell) (version 0.7.5)

LiquidCrystal I2C by Frank de Barbender (for the display) (version 1.1.2)

Any custom local libraries included in this repository. 

3. Hardware Calibration
Before deploying, the HX711 scale must be calibrated. Open modules.h and update the constants:

SCALE_EMPTY_RAW: Your scale's raw output when completely empty. Value Used: 232000, update if necessary using LoadCellTest.ino.

SCALE_CAL_FACTOR: The calibration divider required to output weight in grams. Value Used: 1000, update if necessary using LoadCellTest.ino.

User Guide
Normal Shopping
Adding an Item: Scan a barcode. The display will prompt Place item on scale!. Place the item in the cart. The system will verify the weight and add the price to the total.

Viewing the Cart: Tap Yellow Button (Short Press, < 1.5s). The display will automatically scroll through your cart.

Removing Items (Mid-Shopping)
Press and hold Yellow Button for 1.5 seconds. The screen will change to == REMOVE MODE ==.

Scan the item you wish to remove.

The display will prompt Remove from scale!. Take the item out of the cart. The total price and expected weight will update.

Checkout & Payment
Double-click Button 11 to enter == CHECKOUT MODE ==.

Scan your items one by one and remove them from the cart to bag them. (The total price will not decrease, allowing you to pay your final bill).

Once the cart is empty (0g) and all software items are cleared, the screen displays Make Payment.

Store Staff must scan the Admin Barcode (ITEM_00006) to authorize the payment, which resets the cart for the next customer.

Security & Troubleshooting
Tamper Alarm: If an object is placed in the cart without being scanned, the alarm will sound continuously. Removing the unauthorized object instantly silences the alarm and restores normal operation.

Admin Lockdown: If a user attempts to remove an item they never scanned, or if they take out the wrong amount of weight during a removal, the system enters a hard lockdown (ALARM! Admin Reqd.). It can only be unlocked by a store associate scanning the Admin Barcode.
