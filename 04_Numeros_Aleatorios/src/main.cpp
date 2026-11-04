/*
 * ============================================================
 *  PROJETO 04 - JOGO DA MEMÓRIA "GENIUS"
 *  Tópico 7 - Funções predefinidas: NÚMEROS ALEATÓRIOS
 *  Placa: Arduino UNO  |  Simulação: Wokwi (VS Code)
 * ============================================================
 *  Componentes:
 *   - 4 LEDs (vermelho, verde, azul, amarelo) + 4 resistores 220 Ω
 *   - 4 botões
 *   - Buzzer
 *   - Display LCD 16x2 com módulo I2C        [NOVO]
 *
 *  O jogo sorteia uma sequência de cores que cresce a cada rodada.
 *  O jogador repete a sequência nos botões.
 *
 *  Funções de números aleatórios exploradas:
 *   random(max)  random(min, max)  randomSeed()
 *   e a técnica de semente com micros() (tempo de reação humano)
 * ============================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ----------------- Pinos -----------------
const int LEDS[4]    = { 9, 10, 11, 12 };       // vermelho, verde, azul, amarelo
const int BOTOES[4]  = { 2, 3, 4, 5 };
const int PINO_BUZZER = 8;

// Cada cor tem uma nota musical (Hz)
const int NOTAS[4]   = { 262, 330, 392, 523 };  // Dó, Mi, Sol, Dó agudo
const char* NOMES[4] = { "Vermelho", "Verde", "Azul", "Amarelo" };

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ----------------- Jogo ------------------
const int MAX_SEQ = 50;
byte sequencia[MAX_SEQ];
int  nivel   = 0;
int  recorde = 0;

// ============================================================
//  DEMONSTRAÇÃO DIDÁTICA: "aleatório" no computador é PSEUDOaleatório
// ============================================================
void demonstracaoSemSemente() {
  Serial.println(F("--- Sem randomSeed(): reinicie a simulacao e compare! ---"));
  Serial.print(F("5 numeros com random(1, 7): "));
  for (int i = 0; i < 5; i++) {
    Serial.print(random(1, 7));          // 1 a 6 (o limite superior NÃO entra)
    Serial.print(' ');
  }
  Serial.println();
  Serial.println(F("Sempre os mesmos numeros! A sequencia depende da SEMENTE."));
}

// Sorteia 600 valores de 0 a 3 e mostra quantas vezes cada um saiu
void histograma() {
  int contagem[4] = { 0, 0, 0, 0 };
  for (int i = 0; i < 600; i++) contagem[random(4)]++;   // random(4) -> 0,1,2 ou 3

  Serial.println(F("--- Distribuicao de 600 sorteios de random(4) ---"));
  for (int c = 0; c < 4; c++) {
    Serial.print(NOMES[c]);
    Serial.print(F("\t"));
    Serial.print(contagem[c]);
    Serial.print(F("\t"));
    for (int k = 0; k < contagem[c] / 10; k++) Serial.print('#');
    Serial.println();
  }
}

// ============================================================
//  SAÍDAS
// ============================================================
void lcdLinhas(const String &l1, const String &l2) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(l1);
  lcd.setCursor(0, 1); lcd.print(l2);
}

void acender(int cor, int tempo) {
  digitalWrite(LEDS[cor], HIGH);
  tone(PINO_BUZZER, NOTAS[cor]);
  delay(tempo);
  digitalWrite(LEDS[cor], LOW);
  noTone(PINO_BUZZER);
}

void somDeErro() {
  for (int f = 400; f >= 100; f -= 30) {
    tone(PINO_BUZZER, f);
    delay(25);
  }
  noTone(PINO_BUZZER);
}

void animacaoInicio() {
  for (int volta = 0; volta < 2; volta++) {
    for (int i = 0; i < 4; i++) acender(i, 90);
  }
}

// ============================================================
//  ENTRADA
// ============================================================
// Espera um botão ser pressionado e devolve o índice (0..3)
int esperarBotao() {
  while (true) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(BOTOES[i]) == LOW) {        // INPUT_PULLUP: pressionado = LOW
        digitalWrite(LEDS[i], HIGH);
        tone(PINO_BUZZER, NOTAS[i]);
        while (digitalRead(BOTOES[i]) == LOW) {}  // espera soltar
        delay(30);                                // debounce
        digitalWrite(LEDS[i], LOW);
        noTone(PINO_BUZZER);
        return i;
      }
    }
  }
}

// ============================================================
//  LÓGICA DO JOGO
// ============================================================
void novaPartida() {
  nivel = 0;
  lcdLinhas("GENIUS", "Aperte um botao");

  // O tempo que o jogador leva para apertar o botão é imprevisível:
  // usamos micros() nesse instante como SEMENTE do gerador.
  esperarBotao();
  unsigned long semente = micros();
  randomSeed(semente);

  Serial.print(F("randomSeed("));
  Serial.print(semente);
  Serial.println(F(")  -> agora cada partida e diferente!"));

  // Sorteia a sequência inteira de uma vez
  Serial.print(F("Sequencia sorteada (gabarito): "));
  for (int i = 0; i < MAX_SEQ; i++) {
    sequencia[i] = random(4);                     // 0..3
    if (i < 15) { Serial.print(sequencia[i]); Serial.print(' '); }
  }
  Serial.println(F("..."));

  animacaoInicio();
  delay(500);
}

void mostrarSequencia() {
  // Fica mais rápido conforme o nível aumenta (nunca abaixo de 150 ms)
  int tempo = max(150, 500 - nivel * 25);
  for (int i = 0; i <= nivel; i++) {
    acender(sequencia[i], tempo);
    delay(tempo / 2);
  }
}

bool jogadorAcertou() {
  for (int i = 0; i <= nivel; i++) {
    int apertado = esperarBotao();
    if (apertado != sequencia[i]) {
      Serial.print(F("Errou! Esperado: "));
      Serial.print(NOMES[sequencia[i]]);
      Serial.print(F("  Apertado: "));
      Serial.println(NOMES[apertado]);
      return false;
    }
  }
  return true;
}

// Mensagens de incentivo sorteadas com random(min, max)
const char* ELOGIOS[] = { "Mandou bem!", "Isso ai!", "Memoria top!", "Show!", "Continue!" };

// ============================================================
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(LEDS[i], OUTPUT);
    pinMode(BOTOES[i], INPUT_PULLUP);
  }
  pinMode(PINO_BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();

  demonstracaoSemSemente();
  histograma();

  novaPartida();
}

void loop() {
  lcdLinhas("Nivel " + String(nivel + 1), "Observe...");
  delay(600);
  mostrarSequencia();

  lcdLinhas("Nivel " + String(nivel + 1), "Sua vez!");

  if (jogadorAcertou()) {
    nivel++;
    lcdLinhas(ELOGIOS[random(0, 5)], "Pontos: " + String(nivel));
    delay(800);

    if (nivel >= MAX_SEQ) {
      lcdLinhas("VOCE ZEROU!", "Parabens!");
      delay(3000);
      novaPartida();
    }
  } else {
    somDeErro();
    recorde = max(recorde, nivel);
    lcdLinhas("Fim! Pontos: " + String(nivel), "Recorde: " + String(recorde));
    delay(2500);
    novaPartida();
  }
}
