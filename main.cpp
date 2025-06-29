/*
  CNC-Pendant-Controller

  Arduino **Pro Micro** (ATmega32u4)

  This sketch converts encoder and axis button inputs to keyboard strokes.
  It was made for PlanetCNC and requires the Expr files from this repo and
  to be configured in the Shortcut settings of PlanetCNC or any other program
  where it is used.

  Pinout (Arduino Pro Micro pins):
  -------------------------------
  - Encoder A:        D2 (outputA)
  - Encoder B:        D3 (outputB)
  - Jog Speed X1:     D5 (speedX1)
  - Jog Speed X10:    D6 (speedX10)
  - Jog Speed X100:   D7 (speedX100)
  - Axis X button:    D8 (axisX)
  - Axis Y button:    D9 (axisY)
  - Axis Z button:    D10 (axisZ)
  - Axis 4 button:    D16 / A2 (axis4)
  - LED indicator:    D15 / A1 (led)
  - Emergency Button: D14 / A0 (emergencyButton)

  Keybinds Output by the Controller:
  ----------------------------------
  - Jog Speed buttons:
      * Speed X1  → F13
      * Speed X10 → F14
      * Speed X100→ F15

  - Encoder rotation with axis selected:
      * Axis X: Left  → F16
                 Right → F17
      * Axis Y: Down  → F19
                 Up    → F18
      * Axis Z: Z-    → F21
                 Z+    → F20

  - Emergency Button:
      * ESC (emergency stop)

  Made by Shy-Doge.
*/

#include <limits.h>
#include "Keyboard.h"
#include <util/atomic.h>

volatile long counter = 0;
long prevCounter = 0;

// Pin definitions (Pro Micro digital pins)
constexpr int outputA         = 2;    // Encoder A (D2)
constexpr int outputB         = 3;    // Encoder B (D3)
constexpr int speedX1         = 5;    // Jog Speed X1 (D5)
constexpr int speedX10        = 6;    // Jog Speed X10 (D6)
constexpr int speedX100       = 7;    // Jog Speed X100 (D7)
constexpr int axisX           = 8;    // Axis X (D8)
constexpr int axisY           = 9;    // Axis Y (D9)
constexpr int axisZ           = 10;   // Axis Z (D10)
constexpr int axis4           = 16;   // Axis 4 (D16 / A2)
constexpr int led             = 15;   // LED indicator (D15 / A1)
constexpr int emergencyButton = 14;   // Emergency Button (D14 / A0)

// Debounce state arrays
bool pinOutput[22] = {};
unsigned char pinIntegrator[22] = {};

constexpr unsigned int debounceTime = 5;
constexpr unsigned int sampleInterval = 50;
constexpr unsigned int pinIntegratorMax = sampleInterval / debounceTime;
unsigned long lastDebounceTime = 0;

bool lastEmergencyState = HIGH;

// Interrupt routine for encoder
void handwheel() {
  if (digitalRead(outputA) != digitalRead(outputB)) {
    counter++;
  } else {
    counter--;
  }
}

void setup() {
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(speedX1, INPUT_PULLUP);
  pinMode(speedX10, INPUT_PULLUP);
  pinMode(speedX100, INPUT_PULLUP);
  pinMode(axisX, INPUT_PULLUP);
  pinMode(axisY, INPUT_PULLUP);
  pinMode(axisZ, INPUT_PULLUP);
  pinMode(axis4, INPUT_PULLUP);
  pinMode(emergencyButton, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  digitalWrite(led, LOW);

  // Serial.begin(9600); // Enable Serial debug output
  Keyboard.begin();      // Start keyboard emulation

  attachInterrupt(digitalPinToInterrupt(outputA), handwheel, CHANGE);
}

void sendAxisSpeed() {
  if (digitalRead(speedX1) == LOW) {
    Keyboard.press(240); // F13
    delay(1);
    Keyboard.release(240);
    // Serial.println("Speed: X1");
  } else if (digitalRead(speedX10) == LOW) {
    Keyboard.press(241); // F14
    delay(1);
    Keyboard.release(241);
    // Serial.println("Speed: X10");
  } else if (digitalRead(speedX100) == LOW) {
    Keyboard.press(242); // F15
    delay(1);
    Keyboard.release(242);
    // Serial.println("Speed: X100");
  }
}

void loop() {
  // Emergency button state check
  bool currentEmergencyState = digitalRead(emergencyButton);
  if (currentEmergencyState != lastEmergencyState) {
    lastEmergencyState = currentEmergencyState;
    Keyboard.press(KEY_ESC);
    Keyboard.release(KEY_ESC);
    // Serial.println("Emergency ESC triggered");
    delay(50); // Delay to prevent multiple triggers
  }

  // Debounce and handle buttons
  if ((millis() - lastDebounceTime) > debounceTime) {
    lastDebounceTime = millis();

    for (int i = 5; i <= 16; i++) {
      if ((digitalRead(i) == LOW) && (pinIntegrator[i] > 0)) {
        pinIntegrator[i]--;
        if (pinIntegrator[i] == 0 && pinOutput[i]) {
          pinOutput[i] = false;

          switch (i) {
            case axisX:
            case axisY:
            case axisZ:
            case axis4:
              counter = 0;
              prevCounter = 0;
              // Serial.println("Axis button pressed → Counter reset");
              break;
            case speedX1:
            case speedX10:
            case speedX100:
              sendAxisSpeed();
              break;
          }
        }
      } else if (pinIntegrator[i] < pinIntegratorMax) {
        pinIntegrator[i]++;
        if ((pinIntegrator[i] == pinIntegratorMax) && !pinOutput[i]) {
          pinOutput[i] = true;
        }
      }
    }
  }

  // Encoder movement processing
  int diffCounter;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    diffCounter = (counter - prevCounter) / 2;
    prevCounter += diffCounter * 2;
  }

  /*
  if (diffCounter != 0) {
    Serial.print("Movement: ");
    Serial.print(diffCounter);
  }
  */

  int totalSteps = abs(diffCounter);

  if (totalSteps > 0) {
    if (diffCounter > 0) {
      if (digitalRead(axisX) == LOW) {
        for (int i = 0; i < totalSteps; i++) {
          Keyboard.press(244);  // F17
          delay(1);
          Keyboard.release(244);
        }
      } else if (digitalRead(axisY) == LOW) {
        for (int i = 0; i < totalSteps; i++) {
          Keyboard.press(245);  // F18
          delay(1);
          Keyboard.release(245);
        }
      } else if (digitalRead(axisZ) == LOW) {
        for (int i = 0; i < totalSteps; i++) {
          Keyboard.press(247);  // F20
          delay(1);
          Keyboard.release(247);
        }
      }
    } else {
      if (digitalRead(axisX) == LOW) {
        for (int i = 0; i < totalSteps; i++) {
          Keyboard.press(243);  // F16
          delay(1);
          Keyboard.release(243);
        }
      } else if (digitalRead(axisY) == LOW) {
        for (int i = 0; i < totalSteps; i++) {
          Keyboard.press(246);  // F19
          delay(1);
          Keyboard.release(246);
        }
      } else if (digitalRead(axisZ) == LOW) {
        for (int i = 0; i < totalSteps; i++) {
          Keyboard.press(248);  // F21
          delay(1);
          Keyboard.release(248);
        }
      }
    }
  }
}
