// Include libraries
#include <LiquidCrystal_I2C.h>
#include <AS5600.h>

// Create LCD object: address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Create AS5600 magnetic sensor object
AS5600 sensor;

// Define which pin the button is connected to
const int buttonPin = 12;     // Button switch connected to digital pin 12

// Define which pin controls the button's LED
const int buttonLedPin = 11;  // Button LED + connected to PWM pin 11

// Set the LED brightness level (0-255)
const int ledBrightness = 100;  // Medium brightness

// Toggle state variables
bool displayOn = false;           // Track if display is on or off
bool lastButtonState = HIGH;      // Previous button state
bool buttonState = HIGH;          // Current button state
unsigned long lastDebounceTime = 0;  // Last time button state changed
unsigned long debounceDelay = 50;    // Debounce time in milliseconds

// Display update timing
unsigned long lastDisplayUpdate = 0;  // Last time display was updated
unsigned long displayUpdateInterval = 250;  // Update display every 250ms

void setup() {
  // Small delay to let power stabilize after boot
  delay(100);
  
  // Initialize the LCD
  lcd.init();
  
  // Give LCD time to fully initialize
  delay(500);
  
  // Initialize the AS5600 sensor
  sensor.begin();
  
  // Configure button pin as INPUT with internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Configure button LED pin as OUTPUT
  pinMode(buttonLedPin, OUTPUT);
  
  // Start with button LED OFF
  analogWrite(buttonLedPin, 0);
  
  // Start with the LCD backlight OFF to save power
  lcd.noBacklight();
  
  // Extra delay before writing text
  delay(100);
  
  // Display initial welcome message (even though backlight is off)
  lcd.setCursor(0, 0);
  lcd.print("Press button to");
  lcd.setCursor(0, 1);
  lcd.print("check sensor");
}

void loop() {
  // Read the current button state
  int reading = digitalRead(buttonPin);
  
  // If the button state changed (due to noise or actual press)
  if (reading != lastButtonState) {
    // Reset the debounce timer
    lastDebounceTime = millis();
  }
  
  // If enough time has passed since last state change
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has actually changed
    if (reading != buttonState) {
      buttonState = reading;
      
      // If button is now pressed (LOW because of pull-up)
      if (buttonState == LOW) {
        // Toggle the display state
        displayOn = !displayOn;
        
        if (displayOn) {
          // Turn on display and LED
          analogWrite(buttonLedPin, ledBrightness);
          lcd.backlight();
          // Force immediate display update
          lastDisplayUpdate = 0;
        } else {
          // Turn off display and LED
          analogWrite(buttonLedPin, 0);
          lcd.noBacklight();
          lcd.clear();
        }
      }
    }
  }
  
  // Save the current reading for next loop
  lastButtonState = reading;
  
  // If display is on, update the sensor reading periodically
  if (displayOn) {
    // Check if it's time to update the display
    if (millis() - lastDisplayUpdate >= displayUpdateInterval) {
      lastDisplayUpdate = millis();
      
      // Clear the screen
      lcd.clear();
      
      // Check if magnet is detected
      int magnetStatus = sensor.readStatus();
      
      // Check if magnet is detected (bit 5 = MD, magnet detected)
      if (magnetStatus & 0x20) {  // 0x20 = bit 5 set means magnet detected
        
        // Read the angle from the sensor
        int rawAngle = sensor.readAngle();  // Returns 0-4095
        float angle = rawAngle * 0.087890625;  // Convert to degrees (360/4096)
        
        // Display angle on LCD
        lcd.setCursor(0, 0);
        lcd.print("Magnet Detected!");
        
        lcd.setCursor(0, 1);
        lcd.print("Angle: ");
        lcd.print(angle, 1);  // Display with 1 decimal place
        lcd.print((char)223);  // Degree symbol
        
      } else {
        // No magnet detected
        lcd.setCursor(0, 0);
        lcd.print("No Magnet");
        lcd.setCursor(0, 1);
        lcd.print("Detected");
      }
    }
  }
  // NO delay() here - loop runs as fast as possible to check button frequently!
}