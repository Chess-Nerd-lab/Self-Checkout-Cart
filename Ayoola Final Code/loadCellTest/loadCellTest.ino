#include "HX711.h"

// Define GPIO pins
const int HX711_dout = 4; // DT
const int HX711_sck = 5;  // SCK

HX711 scale;

// Start with a default calibration factor
float calibration_factor = 1000.0; 

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("HX711 Calibration Sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place a known weight on the scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.begin(HX711_dout, HX711_sck);
  
  // Reset the scale to 0
  scale.set_scale();
  scale.tare(); 
  
  long zero_factor = scale.read_average(); // Get a baseline reading
  Serial.print("Zero factor: "); // This can be used to remove the need to tare later
  Serial.println(zero_factor);
}

void loop() {
  // Apply the current calibration factor
  scale.set_scale(calibration_factor);

  Serial.print("Reading: ");
  // Print the weight to 1 decimal place
  Serial.print(scale.get_units(10), 1); 
  Serial.print(" g");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  // Listen for user input from the Serial Monitor to adjust the factor
  if(Serial.available()) {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a') {
      calibration_factor += 10;
    } else if(temp == '-' || temp == 'z') {
      calibration_factor -= 10;
    } else if(temp == 'A') { // Bigger steps
      calibration_factor += 100;
    } else if(temp == 'Z') {
      calibration_factor -= 100;
    }
  }
}