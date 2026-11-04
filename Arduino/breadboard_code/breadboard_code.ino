#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ── Pin definitions ──
const int buttonPin     = 2;
const int buttonLedPin  = 11;
const int hallSensorPin = A0;

// ── LED brightness ──
const int ledBrightness = 100;

// ── Calibration endpoints ──
const int RAW_EMPTY = 735;
const int RAW_FULL  = 515;

// ── Sampling ──
const int NUM_SAMPLES = 15;

// ── State ──
bool displayOn   = false;
bool needsRedraw = true;

// Button debounce
bool lastButtonState = HIGH;
bool buttonState     = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Display update timer
unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 250;

// ── Smoothed sensor read ──
int getSmoothedRaw() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(hallSensorPin);
    delay(2);
  }
  return (int)(sum / NUM_SAMPLES);
}

// ── Turn display off (idle screen) ──
void turnOff() {
  displayOn   = false;
  needsRedraw = true;
  analogWrite(buttonLedPin, 0);
  lcd.noBacklight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Propane");
  lcd.setCursor(0, 1);
  lcd.print("Professionals");
}

// ── Turn display on ──
void turnOn() {
  displayOn         = true;
  needsRedraw       = true;
  lastDisplayUpdate = 0;
  lcd.backlight();
  analogWrite(buttonLedPin, ledBrightness);
}

void setup() {
  lcd.init();
  delay(500);

  pinMode(buttonPin,     INPUT_PULLUP);
  pinMode(buttonLedPin,  OUTPUT);
  pinMode(hallSensorPin, INPUT);

  turnOff();  // Start in idle state
}

void loop() {
  // ── Button debounce ──
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        if (displayOn) turnOff();
        else           turnOn();
      }
    }
  }

  lastButtonState = reading;

  // ── Display update (only when on) ──
  if (displayOn && (millis() - lastDisplayUpdate >= displayUpdateInterval)) {
    lastDisplayUpdate = millis();

    int rawValue = getSmoothedRaw();
    int percent  = map(rawValue, RAW_EMPTY, RAW_FULL, 0, 100);
    percent = constrain(percent, 0, 100);

    if (needsRedraw) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Propane Left:");
      needsRedraw = false;
    }

    lcd.setCursor(0, 1);
    if (percent < 100) lcd.print(" ");
    if (percent < 10)  lcd.print(" ");
    lcd.print(percent);
    lcd.print("%   ");
  }
}