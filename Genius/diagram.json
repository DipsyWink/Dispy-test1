#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
const int inverm = 12; const int inverd = 14; const int inazul = 27; const int inamar = 26;
const int outverm = 0; const int outverd = 4; const int outazul = 16; const int outamar = 17;
const int buzzer = 19;
 
typedef enum ESTADOS { STARTUP, TAG_SELECTION, GAME, GAMEOVER } estados;
estados estadoAtual;
 
int GameState = 0;    
int cores[100];          
int nivel = 0;        
int atual = 0;        
int keyDown = 0;      
float tempoBase = 400.0;
 
char tag[4] = {'A', 'A', 'A', '\0'};
int tagIndex = 0;
 
 
void mostraCor(int cor, int duracao) {
  if(cor == 1) { digitalWrite(outverm, HIGH); tone(buzzer, 440); }
  else if(cor == 2) { digitalWrite(outverd, HIGH); tone(buzzer, 523); }
  else if(cor == 3) { digitalWrite(outazul, HIGH); tone(buzzer, 659); }
  else if(cor == 4) { digitalWrite(outamar, HIGH); tone(buzzer, 784); }
  
  delay(duracao);
  
  digitalWrite(outverm, LOW); digitalWrite(outverd, LOW);
  digitalWrite(outazul, LOW); digitalWrite(outamar, LOW);
  noTone(buzzer);
}
 
int leInput() {
  if(digitalRead(inverm)) return 1;
  if(digitalRead(inverd)) return 2;
  if(digitalRead(inazul)) return 3;
  if(digitalRead(inamar)) return 4;
  return 0;
}
 
void runStartup() {
  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("INICIANDO");
  lcd.setCursor(3, 1); lcd.print("GENIUS <3!");
  delay(1500);
  lcd.clear();
  estadoAtual = TAG_SELECTION;
}
 
void runTagSelection() {
  lcd.setCursor(0, 0);
  lcd.print("Escolha sua TAG:");
  
  lcd.setCursor(5, 1);
  for (int i = 0; i < 3; i++) {
    if (i == tagIndex) {
      lcd.print("["); lcd.print(tag[i]); lcd.print("]");
    } else {
      lcd.print(" "); lcd.print(tag[i]); lcd.print(" ");
    }
  }
 
  int btn = leInput();
  if (btn != 0 && keyDown == 0) {
    keyDown = 1;
    tone(buzzer, 1000, 50);
 
    if (btn == 1) {
      tag[tagIndex]++;
      if (tag[tagIndex] > 'Z') tag[tagIndex] = 'A';
    }
    else if (btn == 2) {
      tag[tagIndex]--;
      if (tag[tagIndex] < 'A') tag[tagIndex] = 'Z';
    }
    else if (btn == 3) {
      if (tagIndex > 0) {
        tagIndex--;
      }
    }
    else if (btn == 4) {
      tagIndex++;
      if (tagIndex >= 3) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Boa sorte!");
        lcd.setCursor(0, 1);
        lcd.print(tag);
        delay(1500);
        lcd.clear();
        
        lcd.setCursor(0, 0); lcd.print("Player: "); lcd.print(tag);
        lcd.setCursor(0, 1); lcd.print("Score: 0");
        estadoAtual = GAME;
      }
    }
  }
  else if (btn == 0) {
    keyDown = 0;
  }
}
 
void runGame() {
  int tempoRodada = max(50, (int)(tempoBase * pow(0.9, nivel)));
  int intervalo = tempoRodada / 2;
 
  if(GameState == 0) {
    delay(800);
    cores[nivel] = random(1, 5);
    for(int i = 0; i <= nivel; i++) {
      mostraCor(cores[i], tempoRodada);
      delay(intervalo);
    }
    GameState = 1;
    atual = 0;
  }
  else {
    int cor = leInput();
    if(cor != 0 && keyDown == 0) {
      keyDown = 1;
      mostraCor(cor, 150);
      if(cor == cores[atual]) {
        atual++;
        if(atual > nivel) {
          nivel++;
          atual = 0;
          GameState = 0;
          lcd.setCursor(7, 1); lcd.print(nivel);
        }
      }
      else { estadoAtual = GAMEOVER; }
    }
    else if(cor == 0) { keyDown = 0; }
  }
}
 
void runGameOver() {
  lcd.clear();
  lcd.setCursor(3, 0); lcd.print("FIM DE JOGO");
  lcd.setCursor(1, 1);
  lcd.print(tag);
  lcd.print(" - Score: ");
  lcd.print(nivel);
 
  tone(buzzer, 150, 1000);
  for(int i = 0; i < 5; i++) {
    digitalWrite(outverm, HIGH); digitalWrite(outverd, HIGH);
    digitalWrite(outazul, HIGH); digitalWrite(outamar, HIGH);
    delay(200);
    digitalWrite(outverm, LOW); digitalWrite(outverd, LOW);
    digitalWrite(outazul, LOW); digitalWrite(outamar, LOW);
    delay(200);
  }
 
  delay(3000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reiniciando...");
  delay(1500);
 
  nivel = 0;
  atual = 0;
  GameState = 0;
  tagIndex = 0;
  tag[0] = 'A'; tag[1] = 'A'; tag[2] = 'A';
  
  estadoAtual = TAG_SELECTION;
}
 
void setup() {
  lcd.init(); lcd.backlight();
  pinMode(inverm, INPUT_PULLDOWN); pinMode(inverd, INPUT_PULLDOWN);
  pinMode(inazul, INPUT_PULLDOWN); pinMode(inamar, INPUT_PULLDOWN);
  pinMode(outverm, OUTPUT); pinMode(outverd, OUTPUT);
  pinMode(outazul, OUTPUT); pinMode(outamar, OUTPUT);
  pinMode(buzzer, OUTPUT);
  randomSeed(analogRead(34));
  estadoAtual = STARTUP;
}
 
void loop() {
  switch(estadoAtual) {
    case STARTUP: runStartup(); break;
    case TAG_SELECTION: runTagSelection(); break;
    case GAME: runGame(); break;
    case GAMEOVER: runGameOver(); break;
  }
}
