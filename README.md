# Dispy-test1

ESP32 Simon-style memory game with LCD display and player TAG identification.

## Hardware

| Component | Pin(s) |
|-----------|--------|
| Button RED (cycle up) | GPIO 12 |
| Button GREEN (cycle down) | GPIO 14 |
| Button BLUE (confirm / OK) | GPIO 27 |
| Button YELLOW (back / cancel) | GPIO 26 |
| LED RED | GPIO 0 |
| LED GREEN | GPIO 4 |
| LED BLUE | GPIO 16 |
| LED YELLOW | GPIO 17 |
| Buzzer | GPIO 19 |
| LCD 16×2 I2C SDA | GPIO 21 |
| LCD 16×2 I2C SCL | GPIO 22 |

LCD I2C address is `0x27` by default (change `#define LCD_ADDR` in `wifi-scan.ino` to `0x3F` if needed).

## Game Flow

1. **Boot** – startup animation plays and a welcome message is shown on the LCD.
2. **TAG Selection** – player uses the four buttons to enter a 3-character TAG (A–Z):
   - **RED** → cycle current character UP (A → B → … → Z → A)
   - **GREEN** → cycle current character DOWN (A → Z → Y → … → A)
   - **BLUE** → confirm character and advance to the next position (starts game after the 3rd character)
   - **YELLOW** → go back to the previous character position
3. **Game** – classic Simon memory game; LCD shows current TAG and score (level).
4. **Game Over** – LCD shows "Game Over", TAG and final score; press any button to return to TAG selection.

## Required Library

### Arduino IDE
Install **LiquidCrystal I2C** by Frank de Brabander via *Sketch → Include Library → Manage Libraries*.

### Wokwi
The `LiquidCrystal_I2C` library is available in the Wokwi simulator automatically.

## Simulation
Open the project in [Wokwi](https://wokwi.com) using the link in `Genius/wokwi-project.txt`.
