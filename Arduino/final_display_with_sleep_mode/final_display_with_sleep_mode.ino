#include <LiquidCrystal_I2C.h>
#include <avr/sleep.h>
#include <avr/power.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ── Pin definitions ──
const int buttonPin     = 2;   
const int buttonLedPin  = 11;
const int hallSensorPin = A0;

// ── LED brightness ──
const int ledBrightness = 100;

// ── Calibration endpoints ──
const int RAW_EMPTY = 735;   // ADC when tank is EMPTY
const int RAW_FULL  = 515;   // ADC when tank is FULL

// ── Sampling ──
const int NUM_SAMPLES = 15;

// ── Auto-shutoff timeout ──
const unsigned long SHUTOFF_TIMEOUT = 30000;  // 30 seconds

// ── State ──
volatile bool wakeFlag    = false;
bool          displayOn   = false;
bool          needsRedraw = true;

// Button debounce
bool lastButtonState = HIGH;
bool buttonState     = HIGH;
unsigned long lastDebounceTime  = 0;
const unsigned long debounceDelay = 50;

// Timers
unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 250;
unsigned long displayOnTime = 0;

void wakeISR() {
  wakeFlag = true;
}

// ── Smoothed sensor read ──
int getSmoothedRaw() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(hallSensorPin);
    delay(2);
  }
  return (int)(sum / NUM_SAMPLES);
}

// ── Turn everything off and enter deep sleep ──
void goToSleep() {
  displayOn = false;
  analogWrite(buttonLedPin, 0);
  lcd.noBacklight();
  lcd.clear();

  delay(50);  // let I2C finish

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(buttonPin), wakeISR, LOW);

  sleep_cpu();

  // Execution resumes here after wake
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(buttonPin));
}

// ── Wake and turn display on ──
void wakeUp() {
  displayOn   = true;
  needsRedraw = true;
  wakeFlag    = false;
  displayOnTime     = millis();
  lastDisplayUpdate = 0;

  lcd.init();
  delay(500);
  lcd.backlight();
  analogWrite(buttonLedPin, ledBrightness);
}

void setup() {
  delay(100);
  lcd.init();
  delay(500);

  pinMode(buttonPin,     INPUT_PULLUP);
  pinMode(buttonLedPin,  OUTPUT);
  pinMode(hallSensorPin, INPUT);

  // Start asleep
  goToSleep();
}

void loop() {
  // ── Handle wake from sleep ──
  if (wakeFlag && !displayOn) {
    wakeUp();
    delay(300);  // ignore bounce right after wake
    lastButtonState = digitalRead(buttonPin);
    buttonState     = lastButtonState;
    return;
  }

  // ── Button debounce ──
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW && displayOn) {
        goToSleep();
        return;
      }
    }
  }

  lastButtonState = reading;

  // ── Auto-shutoff ──
  if (displayOn && (millis() - displayOnTime >= SHUTOFF_TIMEOUT)) {
    goToSleep();
    return;
  }

  // ── Display update ──
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

    // Percentage on second line, padded to avoid leftover chars
    lcd.setCursor(0, 1);
    if (percent < 100) lcd.print(" ");
    if (percent < 10)  lcd.print(" ");
    lcd.print(percent);
    lcd.print("%   ");
  }
}