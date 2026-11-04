/*
 * ============================================================
 *  PROJETO 01 - ESTAÇÃO DE CONFORTO TÉRMICO
 *  Tópico 7 - Funções predefinidas: FUNÇÕES MATEMÁTICAS
 *  Placa: ESP32 DevKit C  |  Simulação: Wokwi (VS Code)
 * ============================================================
 *  Componentes:
 *   - DHT22 (sensor de temperatura e umidade)      [NOVO]
 *   - Display LCD 16x2 com módulo I2C               [NOVO]
 *   - Potenciômetro (seleciona a tela exibida)
 *
 *  Funções matemáticas exploradas:
 *   pow()  sqrt()  sq()  log()  abs()  min()  max()
 *   round()  floor()  ceil()  constrain()  map()
 * ============================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// ----------------- Pinos -----------------
#define PINO_DHT   15     // dados do DHT22
#define PINO_POT   34     // potenciômetro (entrada analógica do ESP32)
#define TIPO_DHT   DHT22

// ----------------- Objetos ---------------
DHT dht(PINO_DHT, TIPO_DHT);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // endereço 0x27, 16 colunas, 2 linhas

// ------------- Histórico (estatística) -------------
const int TAM_HIST = 10;
float historico[TAM_HIST];
int   qtdLeituras = 0;     // quantas leituras válidas já guardamos
int   posHist     = 0;     // próxima posição a ser gravada (buffer circular)

float tempMin =  1000.0;
float tempMax = -1000.0;

unsigned long ultimaLeitura = 0;
const unsigned long INTERVALO_LEITURA = 2000;  // DHT22: no mínimo 2 s entre leituras

float temperatura = 0;
float umidade     = 0;

// ============================================================
//  FUNÇÕES DE CÁLCULO (cada uma usa funções matemáticas)
// ============================================================

// Converte Celsius -> Fahrenheit : F = C * 9/5 + 32
float celsiusParaFahrenheit(float c) {
  return c * 9.0 / 5.0 + 32.0;
}

float fahrenheitParaCelsius(float f) {
  return (f - 32.0) * 5.0 / 9.0;
}

/*
 * Índice de calor (sensação térmica no calor) - fórmula de Rothfusz (NOAA).
 * Usa pow() para elevar T e UR ao quadrado.
 */
float indiceDeCalor(float tempC, float ur) {
  float T = celsiusParaFahrenheit(tempC);

  // Fórmula simplificada (válida para valores baixos)
  float hi = 0.5 * (T + 61.0 + (T - 68.0) * 1.2 + ur * 0.094);

  if (hi >= 80.0) {
    hi = -42.379
         + 2.04901523  * T
         + 10.14333127 * ur
         - 0.22475541  * T * ur
         - 0.00683783  * pow(T, 2)
         - 0.05481717  * pow(ur, 2)
         + 0.00122874  * pow(T, 2) * ur
         + 0.00085282  * T * pow(ur, 2)
         - 0.00000199  * pow(T, 2) * pow(ur, 2);
  }
  return fahrenheitParaCelsius(hi);
}

/*
 * Ponto de orvalho - fórmula de Magnus.
 * Usa log() (logaritmo natural, base e).
 */
float pontoDeOrvalho(float tempC, float ur) {
  const float a = 17.62;
  const float b = 243.12;
  float gama = log(ur / 100.0) + (a * tempC) / (b + tempC);
  return (b * gama) / (a - gama);
}

// Média aritmética do histórico
float media() {
  if (qtdLeituras == 0) return 0;
  float soma = 0;
  for (int i = 0; i < qtdLeituras; i++) soma += historico[i];
  return soma / qtdLeituras;
}

/*
 * Desvio padrão: raiz quadrada da média dos quadrados das diferenças.
 * Usa sq() (quadrado) e sqrt() (raiz quadrada).
 */
float desvioPadrao() {
  if (qtdLeituras < 2) return 0;
  float m = media();
  float somaQuadrados = 0;
  for (int i = 0; i < qtdLeituras; i++) {
    somaQuadrados += sq(historico[i] - m);
  }
  return sqrt(somaQuadrados / qtdLeituras);
}

void guardarNoHistorico(float valor) {
  historico[posHist] = valor;
  posHist = (posHist + 1) % TAM_HIST;          // operador % mantém o índice entre 0 e 9
  if (qtdLeituras < TAM_HIST) qtdLeituras++;

  tempMin = min(tempMin, valor);               // min() devolve o menor
  tempMax = max(tempMax, valor);               // max() devolve o maior
}

// ============================================================
//  SAÍDAS
// ============================================================

void imprimirRelatorioSerial() {
  float m = media();
  Serial.println(F("------------------------------------------------"));
  Serial.printf("Temperatura .......... %.2f C  (%.2f F)\n", temperatura, celsiusParaFahrenheit(temperatura));
  Serial.printf("Umidade relativa ..... %.1f %%\n", umidade);
  Serial.printf("Indice de calor ...... %.2f C   [pow()]\n", indiceDeCalor(temperatura, umidade));
  Serial.printf("Ponto de orvalho ..... %.2f C   [log()]\n", pontoDeOrvalho(temperatura, umidade));
  Serial.printf("round(T)=%.0f  floor(T)=%.0f  ceil(T)=%.0f\n", round(temperatura), floor(temperatura), ceil(temperatura));
  Serial.printf("Min=%.1f  Max=%.1f  Amplitude=%.1f   [min() max()]\n", tempMin, tempMax, tempMax - tempMin);
  Serial.printf("Media(%d)=%.2f  DesvPad=%.2f   [sq() sqrt()]\n", qtdLeituras, m, desvioPadrao());
  Serial.printf("Diferenca da media ... %.2f C   [abs()]\n", abs(temperatura - m));
}

// Cada "tela" do LCD mostra um grupo de cálculos
void mostrarTela(int tela) {
  char linha1[17];
  char linha2[17];

  switch (tela) {
    case 0:  // Leitura direta
      snprintf(linha1, sizeof(linha1), "T:%5.1fC %5.1fF", temperatura, celsiusParaFahrenheit(temperatura));
      snprintf(linha2, sizeof(linha2), "Umidade: %5.1f%%", umidade);
      break;

    case 1:  // Sensação térmica e orvalho
      snprintf(linha1, sizeof(linha1), "Sensacao:%5.1fC", indiceDeCalor(temperatura, umidade));
      snprintf(linha2, sizeof(linha2), "Orvalho: %5.1fC", pontoDeOrvalho(temperatura, umidade));
      break;

    case 2:  // Mínimo / máximo
      snprintf(linha1, sizeof(linha1), "Mn%5.1f Mx%5.1f", tempMin, tempMax);
      snprintf(linha2, sizeof(linha2), "Amplitude:%5.1fC", tempMax - tempMin);
      break;

    default: // Estatística
      snprintf(linha1, sizeof(linha1), "Media:  %6.2fC", media());
      snprintf(linha2, sizeof(linha2), "DesvPad:%6.2fC", desvioPadrao());
      break;
  }

  lcd.setCursor(0, 0);
  lcd.print(linha1);
  for (int i = strlen(linha1); i < 16; i++) lcd.print(' ');   // limpa sobras sem piscar
  lcd.setCursor(0, 1);
  lcd.print(linha2);
  for (int i = strlen(linha2); i < 16; i++) lcd.print(' ');
}

// ============================================================
void setup() {
  Serial.begin(115200);
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Estacao Termica");
  lcd.setCursor(0, 1);
  lcd.print("Funcoes Matemat.");
  delay(1500);
  lcd.clear();

  Serial.println(F("Estacao de Conforto Termico - Funcoes Matematicas"));
  Serial.println(F("Gire o potenciometro para trocar a tela do LCD."));
  Serial.println(F("Clique no DHT22 durante a simulacao para mudar T e UR."));
}

void loop() {
  // --- 1. Ler o sensor a cada 2 segundos ---
  if (millis() - ultimaLeitura >= INTERVALO_LEITURA) {
    ultimaLeitura = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {               // isnan(): verifica "Not a Number"
      Serial.println(F("Falha na leitura do DHT22!"));
    } else {
      temperatura = t;
      umidade     = constrain(h, 1.0, 100.0); // evita log(0) no ponto de orvalho
      guardarNoHistorico(temperatura);
      imprimirRelatorioSerial();
    }
  }

  // --- 2. Potenciômetro escolhe a tela (0 a 3) ---
  int leituraPot = analogRead(PINO_POT);                 // ESP32: 0 a 4095
  int tela = map(leituraPot, 0, 4095, 0, 3);             // reescala para 0..3
  tela = constrain(tela, 0, 3);                          // garante o intervalo

  if (qtdLeituras > 0) mostrarTela(tela);
  delay(100);
}
