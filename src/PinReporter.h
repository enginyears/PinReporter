#pragma once

#include <Arduino.h>

/*
  PinReporter
  -----------
  ESP32 Arduino helper that reports explicitly watched pins using:

    <PR>{"t":...,"pins":[...]}</PR>

  IMPORTANT:
  This first public version keeps the reporting protocol compatible with the
  original project. It does not try to inspect arbitrary C++ code or magically
  discover variables. Pins are registered with PR.watch(...), while PR.begin()
  and PR.update() handle the reporting.

  Supported pin types:
    - digital input
    - digital output
    - PWM
    - analog input

  ESP32 Arduino Core 3.x:
    LEDC is addressed by GPIO pin, so PWM is read with ledcRead(pin).
*/

#define PR_MAX_PINS 32

enum PRPinType : uint8_t {
  PR_DIGITAL_IN,
  PR_DIGITAL_OUT,
  PR_PWM,
  PR_ANALOG_IN
};

struct PRPinInfo {
  uint8_t pin;
  PRPinType type;
  const char* label;
};

class PinReporter {
public:
  // Start the reporter. Default report interval is 150 ms.
  void begin(unsigned long intervalMs = 150) {
    _interval = intervalMs;
    _last = 0;
    _started = true;
  }

  // Register a pin without requiring a configuration array or count.
  // If the pin already exists, its type/label are updated.
  bool watch(uint8_t pin, PRPinType type, const char* label = nullptr) {
    if (!_started) {
      // Allow watch() before begin(); begin() will still start reporting.
    }

    for (uint8_t i = 0; i < _count; ++i) {
      if (_pins[i].pin == pin) {
        _pins[i].type = type;
        if (label != nullptr) {
          _pins[i].label = label;
        }
        return true;
      }
    }

    if (_count >= PR_MAX_PINS) {
      return false;
    }

    _pins[_count].pin = pin;
    _pins[_count].type = type;
    _pins[_count].label = label;
    ++_count;

    return true;
  }

  bool digitalInput(uint8_t pin, const char* label = nullptr) {
    return watch(pin, PR_DIGITAL_IN, label);
  }

  bool digitalOutput(uint8_t pin, const char* label = nullptr) {
    return watch(pin, PR_DIGITAL_OUT, label);
  }

  bool analogInput(uint8_t pin, const char* label = nullptr) {
    return watch(pin, PR_ANALOG_IN, label);
  }

  bool pwm(uint8_t pin, const char* label = nullptr) {
    return watch(pin, PR_PWM, label);
  }

  // Call once per loop(). It sends a report only at the configured interval.
  void update() {
    if (!_started || _count == 0) {
      return;
    }

    const unsigned long now = millis();

    if (now - _last < _interval) {
      return;
    }

    _last = now;
    send();
  }

  uint8_t count() const {
    return _count;
  }

private:
  PRPinInfo _pins[PR_MAX_PINS] = {};
  uint8_t _count = 0;
  unsigned long _interval = 150;
  unsigned long _last = 0;
  bool _started = false;

  const char* typeString(PRPinType type) const {
    switch (type) {
      case PR_DIGITAL_IN:
      case PR_DIGITAL_OUT:
        return "d";

      case PR_PWM:
        return "p";

      case PR_ANALOG_IN:
        return "a";

      default:
        return "d";
    }
  }

  long readValue(const PRPinInfo& p) const {
    switch (p.type) {
      case PR_DIGITAL_IN:
      case PR_DIGITAL_OUT:
        return digitalRead(p.pin);

      case PR_PWM:
#if defined(ESP32)
        // ESP32 Arduino Core 3.x: LEDC is read back by GPIO pin.
        return ledcRead(p.pin);
#else
        return 0;
#endif

      case PR_ANALOG_IN:
        return analogRead(p.pin);

      default:
        return 0;
    }
  }

  void printJsonString(const char* text) const {
    // Labels are normally simple identifiers. Escape the characters that
    // could otherwise make the JSON invalid.
    if (text == nullptr) {
      return;
    }

    for (const char* p = text; *p != '\0'; ++p) {
      if (*p == '"' || *p == '\\') {
        Serial.print('\\');
      }
      Serial.print(*p);
    }
  }

  void send() {
    // Keep these markers exactly compatible with the original website parser.
    Serial.print(F("<PR>{\"t\":"));
    Serial.print(millis());
    Serial.print(F(",\"pins\":["));

    for (uint8_t i = 0; i < _count; ++i) {
      const PRPinInfo& p = _pins[i];

      Serial.print(F("{\"pin\":"));
      Serial.print(p.pin);

      Serial.print(F(",\"type\":\""));
      Serial.print(typeString(p.type));
      Serial.print(F("\",\"val\":"));
      Serial.print(readValue(p));

      if (p.label != nullptr && p.label[0] != '\0') {
        Serial.print(F(",\"label\":\""));
        printJsonString(p.label);
        Serial.print(F("\""));
      }

      Serial.print(F("}"));

      if (i < _count - 1) {
        Serial.print(',');
      }
    }

    Serial.print(F("]}</PR>"));
    Serial.println();
  }
};

// Global object used by the simple public API:
//   PR.begin();
//   PR.watch(...);
//   PR.update();
inline PinReporter PR;
