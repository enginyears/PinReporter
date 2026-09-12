# PinReporter

PinReporter is a lightweight ESP32 Arduino library that reports GPIO state to a
companion web dashboard.

The reporter uses a simple machine-readable frame:

```text
<PR>{...JSON...}</PR>
```

The `<PR>` markers are intentional. A companion website can ignore all normal
Serial output and process only these frames.

## Current status

This is the initial `0.1.0` library structure.

The current implementation provides:

- Digital input reporting
- Digital output reporting
- Analog input reporting
- PWM reporting
- `digitalRead()`
- `digitalWrite()`
- `analogRead()`
- `analogWrite()` / PWM
- Automatic internal pin counting
- No configuration array
- No manual pin count in `begin()`
- `<PR>...</PR>` protocol compatibility

## Supported platform

The initial release targets:

- ESP32
- Arduino framework
- ESP32 Arduino Core 3.x

Other Arduino architectures may be supported in future releases.

---

# Quick start

Include the library:

```cpp
#include <PinReporter.h>
```

Start PinReporter:

```cpp
PR.begin();
```

Register the pins you want to show:

```cpp
PR.digitalOutput(2, "onboard_led");
PR.digitalInput(4, "button");
PR.analogInput(34, "sensor");
PR.pwm(5, "fan");
```

Then call:

```cpp
PR.update();
```

once per `loop()` iteration.

## Complete example

```cpp
#include <PinReporter.h>

const uint8_t LED_PIN = 2;
const uint8_t BUTTON_PIN = 4;
const uint8_t POT_PIN = 34;
const uint8_t PWM_PIN = 5;

void setup() {
  Serial.begin(115200);

  PR.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  PR.digitalOutput(LED_PIN, "onboard_led");
  PR.digitalInput(BUTTON_PIN, "button");
  PR.analogInput(POT_PIN, "potentiometer");
  PR.pwm(PWM_PIN, "pwm_output");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

  int button = digitalRead(BUTTON_PIN);
  int sensor = analogRead(POT_PIN);

  int pwm = map(sensor, 0, 4095, 0, 255);
  analogWrite(PWM_PIN, pwm);

  PR.update();
}
```

---

# Pin definitions remain normal Arduino code

You do **not** need to create a special PinReporter variable for every pin.

This is completely normal:

```cpp
const int LED_PIN = 2;
const int BUTTON_PIN = 4;
const int SENSOR_PIN = 34;
```

You then use the variables normally:

```cpp
pinMode(LED_PIN, OUTPUT);
digitalWrite(LED_PIN, HIGH);

int button = digitalRead(BUTTON_PIN);
int sensor = analogRead(SENSOR_PIN);
```

PinReporter only needs to know which pins should be included in the dashboard.

---

# Registering pins

## Digital output

```cpp
PR.digitalOutput(LED_PIN, "onboard_led");
```

## Digital input

```cpp
PR.digitalInput(BUTTON_PIN, "button");
```

## Analog input

```cpp
PR.analogInput(SENSOR_PIN, "sensor");
```

## PWM

```cpp
PR.pwm(PWM_PIN, "fan");
```

There is no array and no count:

```cpp
// Old approach - not required anymore
PRPinConfig monitoredPins[] = { ... };
PinRep.begin(monitoredPins, 1, 150);
```

The library keeps its own internal list.

---

# Reporting protocol

PinReporter writes frames like:

```text
<PR>{"t":12345,"pins":[{"pin":2,"type":"d","val":1,"label":"onboard_led"}]}</PR>
```

Normal Serial output is still allowed:

```cpp
Serial.println("My debug message");
```

The companion dashboard can simply look for:

```text
<PR>
```

and:

```text
</PR>
```

and ignore everything else.

This means PinReporter does not require the Serial port to be used exclusively by the dashboard.

## Data types

| Type | Meaning |
|---|---|
| `d` | Digital |
| `a` | Analog input |
| `p` | PWM |

The protocol intentionally retains these short type identifiers for compatibility
with the original dashboard.

---

# Installation

## Arduino IDE

### Development installation

Download or clone this repository.

In Arduino IDE:

```text
Sketch
→ Include Library
→ Add .ZIP Library...
```

Select the downloaded PinReporter ZIP.

After the library is published to the Arduino Library Manager, it can be
installed directly by searching for:

```text
PinReporter
```

## PlatformIO

During development, add the GitHub repository to `platformio.ini`:

```ini
lib_deps =
    https://github.com/YOUR_USERNAME/PinReporter.git
```

Replace `YOUR_USERNAME` with the GitHub account that owns this repository.

After publishing the library to the PlatformIO Registry, users can install it
by its registry name.

---

# Repository structure

```text
PinReporter/
├── src/
│   └── PinReporter.h
│
├── examples/
│   └── Basic/
│       └── Basic.ino
│
├── library.properties
├── keywords.txt
├── README.md
├── LICENSE
└── .gitignore
```

---

# Important: automatic pin discovery

The long-term goal of PinReporter is to make the user experience as automatic
as possible.

The ideal user experience is:

```cpp
#include <PinReporter.h>

void setup() {
  Serial.begin(115200);
  PR.begin();

  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH);

  PR.update();
}
```

However, a normal C++ Arduino library cannot inspect arbitrary C++ source code
and automatically discover every GPIO variable used by a sketch.

The current release therefore uses explicit registration:

```cpp
PR.digitalOutput(2);
```

without requiring configuration arrays or pin counts.

Future versions may provide more automatic GPIO interception while preserving
normal Arduino APIs.

---

# ESP32 analog output note

On ESP32:

```cpp
analogRead(pin)
```

reads the ADC.

But:

```cpp
analogWrite(pin, value)
```

does not produce a true analog voltage. It produces PWM.

For that reason PinReporter reports `analogWrite()` pins as PWM (`p`).

---

# Example Serial output

With the Basic example running, you should see ordinary Serial output containing
frames similar to:

```text
<PR>{"t":150,"pins":[{"pin":2,"type":"d","val":0,"label":"onboard_led"},{"pin":4,"type":"d","val":1,"label":"button"},{"pin":34,"type":"a","val":2034,"label":"potentiometer"},{"pin":5,"type":"p","val":127,"label":"pwm_output"}]}</PR>
```

The exact values depend on the hardware and current pin states.

---

# Roadmap

Possible future features:

- Automatic GPIO discovery
- Automatic detection of `pinMode()`
- Automatic detection of `digitalRead()` / `digitalWrite()`
- Automatic detection of `analogRead()`
- Automatic detection of PWM
- ESP32 board information
- Wi-Fi status
- Heap/free memory
- CPU information
- Temperature where supported
- Sensor reporting
- Dashboard commands
- Remote GPIO control
- Multiple transport protocols
- More Arduino architectures

---

# License

MIT License.
