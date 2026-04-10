const int inverm = 12;
const int inverd = 14;
const int inazul = 27;
const int inamar = 26;
const int outverm = 0;
const int outverd = 4;
const int outazul = 16;
const int outamar = 17;

const int buzzer = 19;

typedef enum ESTADOS{
  STARTUP,
  GAME,
  GAMEOVER
}estados;

estados estadoAtual;

void runStartup(){
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
  estadoAtual = GAMEOVER;
}

int GameState = 0;
int cores[10] = {0,0,0,0,0,0,0,0,0,0};
int nivel = 0;
int atual = 0;

void mostraCor(int cor){
  if(cor == 1){
    digitalWrite(outverm, HIGH);
    tone(buzzer, 440);
  }
  else if(cor == 2){
    digitalWrite(outverd, HIGH);
    tone(buzzer, 420);
  }
  else if(cor == 3){
    digitalWrite(outazul, HIGH);
    tone(buzzer, 400);
  }
  else if(cor == 4){
    digitalWrite(outamar, HIGH);
    tone(buzzer, 460);
  }
  delay(200);
  digitalWrite(outverm, LOW);
  digitalWrite(outverd, LOW);
  digitalWrite(outazul, LOW);
  digitalWrite(outamar, LOW);
  noTone(buzzer);
  estadoAtual = GAME;
}

int leInput(){
  if(digitalRead(inverm)) return 1;
  if(digitalRead(inverd)) return 2;
  if(digitalRead(inazul)) return 3;
  if(digitalRead(inamar)) return 4;
  return 0;
}

void runGame(){
  if(GameState == 0){
    Serial.print("Gamestate 0");
    cores[nivel] = random(1,4);
    for(int i = 0; i <= nivel; i++){
      mostraCor(cores[i]);
    }
    GameState = 1;
  }
  else{
    if(atual <= nivel){
      int cor = leInput();
      if(cor != 0 && cor == cores[nivel]){
        atual++;
      }
      else{
        estadoAtual = GAMEOVER;
      }
    }
    else{
      GameState = 0;
      nivel++;
      atual = 0;
    }
  }
}

void runGameOver(){
  tone(buzzer, 220);
  for(int i = 0; i < 4; i++){
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
}

void setup() {

  estadoAtual = STARTUP;
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(inverm, INPUT_PULLDOWN);
  pinMode(inverd, INPUT_PULLDOWN);
  pinMode(inazul, INPUT_PULLDOWN);
  pinMode(inamar, INPUT_PULLDOWN);

  pinMode(outverm, OUTPUT);
  pinMode(outverd, OUTPUT);
  pinMode(outazul, OUTPUT);
  pinMode(outamar, OUTPUT);

  pinMode(buzzer, OUTPUT);
}

void loop() {
  switch(estadoAtual){
    case STARTUP:
      runStartup();
      break;
    case GAME:
      runGame();
      break;
    case GAMEOVER:
      runGameOver();
      break;
  }
}