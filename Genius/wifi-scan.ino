#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD I2C address (0x27 is most common; change to 0x3F if needed)
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Button pin definitions (Inputs)
// inverm = RED   -> TAG: cycle character UP
// inverd = GREEN -> TAG: cycle character DOWN
// inazul = BLUE  -> TAG: confirm character / start game
// inamar = YELLOW-> TAG: go back one position
const int inverm = 12;
const int inverd = 14;
const int inazul = 27;
const int inamar = 26;

// LED pin definitions (Outputs)
const int outverm = 0;
const int outverd = 4;
const int outazul = 16;
const int outamar = 17;

const int buzzer = 19;

// State machine
typedef enum ESTADOS {
  STARTUP = 0,
  TAG_SELECT,  // Player enters 3-character TAG before game starts
  GAME,
  GAMEOVER
} estados;

estados estadoAtual;

// ---- TAG selection variables ----
// Characters cycle A-Z (26 chars)
char playerTag[4] = "___";  // 3-char TAG + null terminator
int tagPos    = 0;           // Current character position being edited (0-2)
int tagChar   = 0;           // Index into A-Z for the current position (0=A, 25=Z)
int tagKeyDown = 0;          // Debounce flag for TAG selection buttons

// ---- Game variables ----
int GameState = 0;
int cores[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int nivel = 0;   // Current level (also used as score)
int atual = 0;   // Index of the colour the player must press next
int keyDown = 0; // Debounce flag for game buttons

// ---------------------------------------------------------------------------
// LCD helper functions
// ---------------------------------------------------------------------------

void lcdWelcome() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Simon Game!");
  lcd.setCursor(2, 1);
  lcd.print("Bem-vindo!");
}

// Shows the TAG selection screen.
// Row 0: "TAG: [X]_  _   " where [X] is the char being edited,
//        confirmed chars are shown plain, unconfirmed positions shown as '_'.
// Row 1: button hints
void lcdTagSelect() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TAG:");
  for (int i = 0; i < 3; i++) {
    if (i == tagPos) {
      // Current position being edited
      lcd.print('[');
      lcd.print((char)('A' + tagChar));
      lcd.print(']');
    } else if (i < tagPos) {
      // Already confirmed position
      lcd.print(' ');
      lcd.print(playerTag[i]);
      lcd.print(' ');
    } else {
      // Not yet reached
      lcd.print(" _ ");
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("^ v OK  BACK");
}

// Shows current score during gameplay.
// Row 0: "TAG: ABC"
// Row 1: "Score: N"
void lcdGameScore() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TAG: ");
  lcd.print(playerTag);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(nivel);
}

// Shows game over screen.
// Row 0: "  Game Over!   "
// Row 1: "TAG:ABC Pts:N  "
void lcdGameOver() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("TAG:");
  lcd.print(playerTag);
  lcd.print(" Pts:");
  lcd.print(nivel);
}

// ---------------------------------------------------------------------------
// State: STARTUP
// Plays startup animation, shows welcome, then transitions to TAG_SELECT.
// ---------------------------------------------------------------------------
void runStartup() {
  lcdWelcome();

  tone(buzzer, 440, 200);
  delay(200);
  digitalWrite(outverm, HIGH);
  tone(buzzer, 455, 200);
  delay(200);
  digitalWrite(outverd, HIGH);
  tone(buzzer, 470, 200);
  delay(200);
  digitalWrite(outazul, HIGH);
  tone(buzzer, 485, 200);
  delay(200);
  digitalWrite(outamar, HIGH);
  tone(buzzer, 500, 200);
  delay(200);
  digitalWrite(outverm, LOW);
  digitalWrite(outverd, LOW);
  digitalWrite(outazul, LOW);
  digitalWrite(outamar, LOW);
  noTone(buzzer);

  delay(1000);  // Hold welcome screen briefly

  // Initialise TAG selection
  tagPos = 0;
  tagChar = 0;
  playerTag[0] = '_';
  playerTag[1] = '_';
  playerTag[2] = '_';
  playerTag[3] = '\0';
  tagKeyDown = 0;

  estadoAtual = TAG_SELECT;
  lcdTagSelect();
}

// ---------------------------------------------------------------------------
// State: TAG_SELECT
// Player uses 4 buttons to build a 3-character TAG (A-Z).
//   RED   (inverm) -> cycle UP   (A->B->...->Z->A)
//   GREEN (inverd) -> cycle DOWN (A->Z->Y->...->A)
//   BLUE  (inazul) -> CONFIRM current char / advance position
//                     (starts game after the 3rd confirmation)
//   YELLOW(inamar) -> BACK (return to previous position)
// ---------------------------------------------------------------------------
void runTagSelect() {
  int btn = 0;
  if      (digitalRead(inverm)) btn = 1;  // UP
  else if (digitalRead(inverd)) btn = 2;  // DOWN
  else if (digitalRead(inazul)) btn = 3;  // CONFIRM
  else if (digitalRead(inamar)) btn = 4;  // BACK

  if (btn != 0 && tagKeyDown == 0) {
    tagKeyDown = 1;

    if (btn == 1) {
      // Cycle character UP: A -> B -> ... -> Z -> A
      tagChar = (tagChar + 1) % 26;
      lcdTagSelect();
    }
    else if (btn == 2) {
      // Cycle character DOWN: A -> Z -> Y -> ... (add 25 to avoid negative modulo)
      tagChar = (tagChar + 25) % 26;
      lcdTagSelect();
    }
    else if (btn == 3) {
      // Confirm current character and advance
      playerTag[tagPos] = 'A' + tagChar;
      tagPos++;
      if (tagPos >= 3) {
        // All 3 characters confirmed — start the game
        playerTag[3] = '\0';
        GameState = 0;
        nivel     = 0;
        atual     = 0;
        keyDown   = 0;
        estadoAtual = GAME;
        lcdGameScore();
      } else {
        // Advance to next position, starting at 'A'
        tagChar = 0;
        lcdTagSelect();
      }
    }
    else if (btn == 4) {
      // Back: return to previous position
      if (tagPos > 0) {
        tagPos--;
        tagChar = playerTag[tagPos] - 'A';
        playerTag[tagPos] = '_';  // Mark as unconfirmed again
        lcdTagSelect();
      }
    }
  }
  else if (btn == 0) {
    tagKeyDown = 0;  // Button released — reset debounce
  }
}

// ---------------------------------------------------------------------------
// Helpers shared by GAME state
// ---------------------------------------------------------------------------

void mostraCor(int cor) {
  if (cor == 1) {
    digitalWrite(outverm, HIGH);
    tone(buzzer, 440);
  } else if (cor == 2) {
    digitalWrite(outverd, HIGH);
    tone(buzzer, 420);
  } else if (cor == 3) {
    digitalWrite(outazul, HIGH);
    tone(buzzer, 400);
  } else if (cor == 4) {
    digitalWrite(outamar, HIGH);
    tone(buzzer, 460);
  }
  delay(200);
  digitalWrite(outverm, LOW);
  digitalWrite(outverd, LOW);
  digitalWrite(outazul, LOW);
  digitalWrite(outamar, LOW);
  noTone(buzzer);
}

// Returns 1-4 for the button pressed, or 0 if none
int leInput() {
  if (digitalRead(inverm)) return 1;
  if (digitalRead(inverd)) return 2;
  if (digitalRead(inazul)) return 3;
  if (digitalRead(inamar)) return 4;
  return 0;
}

// ---------------------------------------------------------------------------
// State: GAME
// ---------------------------------------------------------------------------
void runGame() {
  if (GameState == 0) {
    // Show the sequence to the player
    Serial.print("Gamestate 0, nivel=");
    Serial.println(nivel);
    cores[nivel] = random(1, 5);  // Pick colour 1-4 for this level's new step
    for (int i = 0; i <= nivel; i++) {
      mostraCor(cores[i]);
      delay(100);
    }
    GameState = 1;
    lcdGameScore();
  } else {
    // Wait for player to reproduce the sequence
    if (atual <= nivel) {
      int cor = leInput();
      if (cor != 0 && keyDown == 0) {
        keyDown = 1;
        if (cor == cores[atual]) {
          atual++;
        } else {
          estadoAtual = GAMEOVER;
        }
      } else if (cor == 0) {
        keyDown = 0;
      }
    } else {
      // Player completed this level — advance
      GameState = 0;
      nivel++;
      atual = 0;
      delay(500);
      lcdGameScore();
    }
  }
}

// ---------------------------------------------------------------------------
// State: GAMEOVER
// Plays failure animation, shows score on LCD, waits for button press,
// then returns to TAG_SELECT so the player can re-enter their TAG.
// ---------------------------------------------------------------------------
void runGameOver() {
  lcdGameOver();

  tone(buzzer, 220);
  for (int i = 0; i < 4; i++) {
    digitalWrite(outverm, HIGH);
    digitalWrite(outverd, HIGH);
    digitalWrite(outazul, HIGH);
    digitalWrite(outamar, HIGH);
    delay(200);
    digitalWrite(outverm, LOW);
    digitalWrite(outverd, LOW);
    digitalWrite(outazul, LOW);
    digitalWrite(outamar, LOW);
    delay(200);
  }
  noTone(buzzer);

  Serial.print("Game Over! TAG: ");
  Serial.print(playerTag);
  Serial.print("  Score: ");
  Serial.println(nivel);

  // Wait for all buttons to be released, then wait for any press to restart
  while (leInput() != 0) { delay(10); }
  while (leInput() == 0) { delay(10); }
  while (leInput() != 0) { delay(10); }  // debounce release

  // Return to TAG selection
  tagPos = 0;
  tagChar = 0;
  playerTag[0] = '_';
  playerTag[1] = '_';
  playerTag[2] = '_';
  playerTag[3] = '\0';
  tagKeyDown = 0;

  estadoAtual = TAG_SELECT;
  lcdTagSelect();
}

// ---------------------------------------------------------------------------
// Arduino setup / loop
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Initialise LCD
  lcd.init();
  lcd.backlight();

  // Button pins
  pinMode(inverm, INPUT_PULLDOWN);
  pinMode(inverd, INPUT_PULLDOWN);
  pinMode(inazul, INPUT_PULLDOWN);
  pinMode(inamar, INPUT_PULLDOWN);

  // LED pins
  pinMode(outverm, OUTPUT);
  pinMode(outverd, OUTPUT);
  pinMode(outazul, OUTPUT);
  pinMode(outamar, OUTPUT);

  pinMode(buzzer, OUTPUT);

  randomSeed(analogRead(0));

  estadoAtual = STARTUP;
}

void loop() {
  switch (estadoAtual) {
    case STARTUP:
      runStartup();
      break;
    case TAG_SELECT:
      runTagSelect();
      break;
    case GAME:
      runGame();
      break;
    case GAMEOVER:
      runGameOver();
      break;
  }
}