// Include the library for I2C LCD
#include <LiquidCrystal_I2C.h>

// Create LCD object: address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define which pin the button is connected to
const int buttonPin = 12;     // Button switch connected to digital pin 12

// Define which pin controls the button's LED
const int buttonLedPin = 11;  // Button LED + connected to PWM pin 11

// Set the LED brightness level (0-255)
const int ledBrightness = 100;  // Medium brightness - adjust as needed

// This runs ONCE when Arduino powers on or resets
void setup() {
  // Small delay to let power stabilize after boot
  delay(100);
  
  // Initialize the LCD
  lcd.init();
  
  // CRITICAL: Give LCD time to fully initialize after power-on
  delay(500);
  
  // Configure button pin as INPUT with internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Configure button LED pin as OUTPUT so we can control it
  pinMode(buttonLedPin, OUTPUT);
  
  // Start with button LED OFF
  analogWrite(buttonLedPin, 0);
  
  // Start with the LCD backlight OFF to save power
  lcd.noBacklight();
  
  // Extra delay before writing text
  delay(100);
  
  // Display initial welcome message (even though backlight is off)
  lcd.setCursor(0, 0);
  lcd.print("Ready to check");
  lcd.setCursor(0, 1);
  lcd.print("propane level");
}

// This runs CONTINUOUSLY in a loop forever
void loop() {
  // Read the button state
  int buttonState = digitalRead(buttonPin);
  
  // Check if button is pressed (reads LOW because of pull-up)
  if (buttonState == LOW) {
    
    // Turn on the button LED at the set brightness
    analogWrite(buttonLedPin, ledBrightness);
    
    // Turn on the LCD backlight so user can see the display
    lcd.backlight();
    
    // Clear any old text from the screen
    lcd.clear();
    
    // Display propane information (placeholder for now)
    lcd.setCursor(0, 0);
    lcd.print("Propane: xx%");
    
    lcd.setCursor(0, 1);
    lcd.print("Time: xx hrs");
    
    // Keep the display on for 10 seconds (10000 milliseconds)
    delay(10000);
    
    // After 10 seconds, turn off the LCD backlight to save power
    lcd.noBacklight();
    
    // Turn off the button LED
    analogWrite(buttonLedPin, 0);
    
    // Clear the display (optional - saves a tiny bit of power)
    lcd.clear();
    
    // Small delay to prevent button "bounce" issues
    delay(100);
  }
}