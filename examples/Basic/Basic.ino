#include <PinReporter.h>

/*
  PinReporter Basic Example
  -------------------------

  This example demonstrates:

    - digitalWrite()
    - digitalRead()
    - analogRead()
    - analogWrite() / PWM
    - PinReporter <PR>...</PR> reporting

  The pin definitions remain normal Arduino variables.

  PinReporter-specific code is deliberately small:

    PR.begin();

    PR.watch(...);

    PR.update();

  The <PR> markers are part of the PinReporter protocol and are consumed
  by the companion dashboard/website.
*/

// ---------------------------------------------------------------------------
// Pin definitions
// ---------------------------------------------------------------------------

const uint8_t LED_PIN = 2;       // Built-in LED on many ESP32 DevKit boards
const uint8_t BUTTON_PIN = 4;   // Push button
const uint8_t POT_PIN = 34;     // Analog input
const uint8_t PWM_PIN = 5;      // PWM output

bool ledState = false;
unsigned long lastBlink = 0;

void setup() {
  Serial.begin(115200);

  // Start PinReporter.
  // Default reporting interval: 150 ms.
  PR.begin();

  // Normal Arduino pin configuration.
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Tell PinReporter which pins this example wants on the dashboard.
  // No array and no pin count are required.
  PR.digitalOutput(LED_PIN, "onboard_led");
  PR.digitalInput(BUTTON_PIN, "button");
  PR.analogInput(POT_PIN, "potentiometer");
  PR.pwm(PWM_PIN, "pwm_output");
}

void loop() {

  // -------------------------------------------------------------------------
  // DIGITAL WRITE
  // -------------------------------------------------------------------------

  if (millis() - lastBlink >= 500) {
    lastBlink = millis();

    ledState = !ledState;

    digitalWrite(LED_PIN, ledState);
  }


  // -------------------------------------------------------------------------
  // DIGITAL READ
  // -------------------------------------------------------------------------

  int buttonState = digitalRead(BUTTON_PIN);

  // This is only here to prove the button is being read.
  // The value is otherwise not needed by the example.
  (void)buttonState;


  // -------------------------------------------------------------------------
  // ANALOG READ
  // -------------------------------------------------------------------------

  int potValue = analogRead(POT_PIN);


  // -------------------------------------------------------------------------
  // ANALOG WRITE / PWM
  // -------------------------------------------------------------------------

  // On ESP32, analogWrite() is PWM output, not a true analog voltage output.
  //
  // The ESP32 ADC is normally 0-4095 and analogWrite() normally uses a
  // 0-255 duty range.
  int pwmValue = map(potValue, 0, 4095, 0, 255);

  analogWrite(PWM_PIN, pwmValue);


  // -------------------------------------------------------------------------
  // PIN REPORTER
  // -------------------------------------------------------------------------

  // The only PinReporter line needed inside loop().
  PR.update();
}
