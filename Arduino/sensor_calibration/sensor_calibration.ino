#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
const int buttonPin     = 2;
const int buttonLedPin  = 11;
const int hallSensorPin = A0;

// LED brightness
const int ledBrightness = 100;

// Calibration endpoints (Update these after getting the sensor in position)
const int RAW_EMPTY = 735;
const int RAW_FULL  = 500;

// Number of samples to average for noise reduction
const int NUM_SAMPLES = 15;

// Button state
bool displayOn = false;
bool lastButtonState = HIGH;
bool buttonState     = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Display refresh timing
unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 150;

bool needsRedraw = true;

int getSmoothedRaw() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(hallSensorPin);
    delay(2);
  }
  return (int)(sum / NUM_SAMPLES);
}

void setup() {
  delay(100);
  lcd.init();
  delay(500);

  pinMode(buttonPin,    INPUT_PULLUP);
  pinMode(buttonLedPin, OUTPUT);
  pinMode(hallSensorPin, INPUT);

  analogWrite(buttonLedPin, 0);
  lcd.noBacklight();

  lcd.setCursor(0, 0);
  lcd.print("Press button to");
  lcd.setCursor(0, 1);
  lcd.print("start calibration");
}

void loop() {
  // --- Button debounce logic ---
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        displayOn = !displayOn;

        if (displayOn) {
          analogWrite(buttonLedPin, ledBrightness);
          lcd.backlight();
          needsRedraw = true;
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

  // --- Display update ---
  if (displayOn) {
    if (millis() - lastDisplayUpdate >= displayUpdateInterval) {
      lastDisplayUpdate = millis();

      int rawValue = getSmoothedRaw();
      int percent  = map(rawValue, RAW_EMPTY, RAW_FULL, 0, 100);
      percent = constrain(percent, 0, 100);

      // Draw static labels once
      if (needsRedraw) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Level:      %");
        lcd.setCursor(0, 1);
        lcd.print("Raw:");
        needsRedraw = false;
      }

      // Update percentage
      lcd.setCursor(7, 0);
      lcd.print("    ");
      lcd.setCursor(7, 0);
      lcd.print(percent);

      // Update raw value
      lcd.setCursor(5, 1);
      lcd.print("     ");
      lcd.setCursor(5, 1);
      lcd.print(rawValue);
    }
  }
}