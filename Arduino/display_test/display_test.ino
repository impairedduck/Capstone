// Include the library for I2C LCD
#include <LiquidCrystal_I2C.h>

// Create LCD object: address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// code here runs when the Arduino starts up VV
void setup() {
  // Initialize the LCD
  lcd.init();
  
  // Turn on the backlight
  lcd.backlight();
  
  // Set cursor to column 0, row 0 (top left)
  lcd.setCursor(0, 0);
  
  // Print message on first row
  lcd.print("Propane");
  
  // Set cursor to column 0, row 1 (bottom row)
  lcd.setCursor(0, 1);
  
  // Print message on second row
  lcd.print("Professionals");
}

void loop() {
}