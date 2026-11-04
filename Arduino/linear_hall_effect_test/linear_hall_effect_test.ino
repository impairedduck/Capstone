// Include library for LCD
#include <LiquidCrystal_I2C.h>

// Create LCD object: address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pin connections
const int buttonPin = 12;        // Button switch
const int buttonLedPin = 11;     // Button LED
const int hallSensorPin = A0;    // SS49E Hall sensor output

// Set the LED brightness level (0-255)
const int ledBrightness = 100;

// Toggle state variables
bool displayOn = false;
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Display update timing
unsigned long lastDisplayUpdate = 0;
unsigned long displayUpdateInterval = 100;

// Track if we need to redraw the static text
bool needsRedraw = true;

void setup() {
  // Small delay to let power stabilize
  delay(100);
  
  // Initialize the LCD
  lcd.init();
  delay(500);
  
  // Configure pins
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonLedPin, OUTPUT);
  pinMode(hallSensorPin, INPUT);
  
  // Start with button LED OFF
  analogWrite(buttonLedPin, 0);
  
  // Start with LCD backlight OFF
  lcd.noBacklight();
  
  delay(100);
  
  // Display welcome message
  lcd.setCursor(0, 0);
  lcd.print("Press button to");
  lcd.setCursor(0, 1);
  lcd.print("test Hall sensor");
}

void loop() {
  // Read the current button state
  int reading = digitalRead(buttonPin);
  
  // Button debouncing
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      
      if (buttonState == LOW) {
        // Toggle display
        displayOn = !displayOn;
        
        if (displayOn) {
          analogWrite(buttonLedPin, ledBrightness);
          lcd.backlight();
          needsRedraw = true;  // Flag to redraw static text
          lastDisplayUpdate = 0;
        } else {
          analogWrite(buttonLedPin, 0);
          lcd.noBacklight();
          lcd.clear();
        }
      }
    }
  }
  
  lastButtonState = reading;
  
  // Update display if it's on
  if (displayOn) {
    if (millis() - lastDisplayUpdate >= displayUpdateInterval) {
      lastDisplayUpdate = millis();
      
      // Read Hall sensor
      int rawValue = analogRead(hallSensorPin);
      float voltage = rawValue * (5.0 / 1023.0);
      
      // Only draw static text once when display turns on
      if (needsRedraw) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Raw:     ");
        lcd.setCursor(0, 1);
        lcd.print("V:       ");
        needsRedraw = false;
      }
      
      // Update only the changing numbers (no clear, no flicker!)
      lcd.setCursor(5, 0);
      lcd.print("    ");  // Clear old number area
      lcd.setCursor(5, 0);
      lcd.print(rawValue);
      
      lcd.setCursor(3, 1);
      lcd.print("     ");  // Clear old number area
      lcd.setCursor(3, 1);
      lcd.print(voltage, 2);
      lcd.print("V");
    }
  }
}
