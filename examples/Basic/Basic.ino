#include <PinReporter.h>

const uint8_t LED_PIN = 2;
const uint8_t BUTTON_PIN = 4;
const uint8_t POT_PIN = 34;
const uint8_t PWM_PIN = 5;

int pwmValue = 0;
bool ledState = false;

unsigned long lastBlink = 0;

void setup() {
  Serial.begin(115200);

  // Start PinReporter.
  // That's all the PinReporter-specific setup required.
  PR.begin();

  // Normal Arduino code.
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Nothing special is required for analogRead().
}

void loop() {
  if (millis() - lastBlink >= 500) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  int buttonState = digitalRead(BUTTON_PIN);
  int potValue = analogRead(POT_PIN);

  // Convert ADC value (0-4095) to PWM value (0-255).
  pwmValue = map(potValue, 0, 4095, 0, 255);
  analogWrite(PWM_PIN, pwmValue);

  // The ONLY PinReporter line required in loop().
  PR.update();
}
