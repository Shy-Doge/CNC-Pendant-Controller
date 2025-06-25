# CNC-Pendant-Controller

Firmware for Arduino **Pro Micro** (ATmega32u4) that converts rotary encoder and axis button inputs into keyboard strokes.  
Designed primarily for use with PlanetCNC, this controller enables efficient jogging and axis selection through customizable keybinds.

---

## Features

- Reads inputs from a rotary encoder and axis buttons.
- Outputs keyboard shortcuts corresponding to jog speed and axis movements.
- Emergency stop button mapped to the ESC key.
- Compatible with PlanetCNC and any program supporting configurable shortcut keys.

---

## Pinout (Arduino Pro Micro)

| Function           | Pin             |
|--------------------|-----------------|
| Encoder A          | D2 (outputA)    |
| Encoder B          | D3 (outputB)    |
| Jog Speed X1       | D5 (speedX1)    |
| Jog Speed X10      | D6 (speedX10)   |
| Jog Speed X100     | D7 (speedX100)  |
| Axis X Button      | D8 (axisX)      |
| Axis Y Button      | D9 (axisY)      |
| Axis Z Button      | D10 (axisZ)     |
| Axis 4 Button      | D16 / A2 (axis4)|
| LED Indicator      | D15 / A1 (led)  |
| Emergency Button   | D14 / A0 (emergencyButton) |

---

## Keybinds Output

### Jog Speed Buttons
- Speed X1  → `F13`
- Speed X10 → `F14`
- Speed X100→ `F15`

### Encoder Rotation (with axis selected)
- **Axis X**  
  - Left  → `F16`  
  - Right → `F17`  
- **Axis Y**  
  - Down  → `F19`  
  - Up    → `F18`  
- **Axis Z**  
  - Z-    → `F21`  
  - Z+    → `F20`  

### Emergency Button
- ESC (Emergency Stop)

---

## Usage

1. Upload the sketch to an Arduino Pro Micro (ATmega32u4).
2. Connect the hardware according to the pinout above.
3. Import the `Expr.txt` files from this repository into PlanetCNC.
4. Configure PlanetCNC's Shortcut settings to match the keybind outputs.
5. Use the pendant to control jogging and axis selection.

---

## Author

Made by **Shy-Doge**.
