/*
 * ============================================================
 *  PROJETO 02 - RADAR ULTRASSÔNICO
 *  Tópico 7 - Funções predefinidas: FUNÇÕES TRIGONOMÉTRICAS
 *  Placa: ESP32 DevKit C  |  Simulação: Wokwi (VS Code)
 * ============================================================
 *  Componentes:
 *   - Servo motor SG90                        [NOVO]
 *   - Sensor ultrassônico HC-SR04             [NOVO]
 *   - Display OLED 128x64 SSD1306 (I2C)       [NOVO]
 *
 *  Ideia: o servo gira o sensor de 0° a 180°. Para cada ângulo
 *  medimos a distância (coordenada POLAR: raio + ângulo) e
 *  convertemos para coordenada CARTESIANA (x, y) usando:
 *        x = d * cos(θ)        y = d * sin(θ)
 *
 *  Funções trigonométricas exploradas:
 *   sin()  cos()  tan()  atan2()  radians()  degrees()
 *   hypot()  e a constante PI
 * ============================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// ----------------- Pinos -----------------
#define PINO_SERVO  18
#define PINO_TRIG    5
#define PINO_ECHO   19

// ----------------- Display ----------------
#define LARGURA 128
#define ALTURA   64
Adafruit_SSD1306 oled(LARGURA, ALTURA, &Wire, -1);

// --------- Geometria do radar na tela ---------
const int   CENTRO_X   = 64;     // centro do semicírculo (meio da tela, embaixo)
const int   CENTRO_Y   = 63;
const int   RAIO_TELA  = 54;     // raio máximo em pixels
const float ALCANCE_CM = 200.0;  // distância representada pelo raio máximo

// --------------- Servo / varredura -------------
Servo servo;
int angulo = 0;
int passo  = 3;                  // graus por passo (+3 ou -3)

// Guarda a distância medida em cada ângulo (0..180) para desenhar os "ecos"
float distancias[181];

// ============================================================
//  SENSOR
// ============================================================
float medirDistanciaCm() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);       // pulso de 10 µs dispara a medição
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  unsigned long duracao = pulseIn(PINO_ECHO, HIGH, 30000);  // timeout 30 ms
  if (duracao == 0) return -1;                              // sem eco

  // velocidade do som ≈ 0,0343 cm/µs ; ida e volta -> divide por 2
  return duracao * 0.0343 / 2.0;
}

// ============================================================
//  CONVERSÕES TRIGONOMÉTRICAS
// ============================================================

// Converte (ângulo em graus, raio em pixels) -> ponto na tela
void polarParaTela(float graus, float raioPx, int &px, int &py) {
  float rad = radians(graus);                 // sin/cos trabalham em RADIANOS
  px = CENTRO_X + round(raioPx * cos(rad));   // eixo x cresce para a direita
  py = CENTRO_Y - round(raioPx * sin(rad));   // eixo y da tela cresce para BAIXO, por isso o "-"
}

// ============================================================
//  DESENHO
// ============================================================
void desenharGrade() {
  // Arcos de 1/3, 2/3 e 3/3 do alcance, desenhados ponto a ponto com sin/cos
  for (int k = 1; k <= 3; k++) {
    float r = RAIO_TELA * k / 3.0;
    for (int g = 0; g <= 180; g += 6) {
      int x, y;
      polarParaTela(g, r, x, y);
      oled.drawPixel(x, y, SSD1306_WHITE);
    }
  }
  // Linhas guia a cada 45°
  for (int g = 0; g <= 180; g += 45) {
    int x, y;
    polarParaTela(g, RAIO_TELA, x, y);
    oled.drawLine(CENTRO_X, CENTRO_Y, x, y, SSD1306_WHITE);
  }
}

void desenharEcos() {
  for (int g = 0; g <= 180; g++) {
    float d = distancias[g];
    if (d > 0 && d <= ALCANCE_CM) {
      float raioPx = d / ALCANCE_CM * RAIO_TELA;   // regra de três: cm -> pixels
      int x, y;
      polarParaTela(g, raioPx, x, y);
      oled.fillCircle(x, y, 1, SSD1306_WHITE);
    }
  }
}

void desenharFeixe(int graus) {
  int x, y;
  polarParaTela(graus, RAIO_TELA, x, y);
  oled.drawLine(CENTRO_X, CENTRO_Y, x, y, SSD1306_WHITE);
}

void desenharTexto(int graus, float d) {
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.printf("%3d%c", graus, (char)247);          // 247 = símbolo de grau na fonte padrão
  oled.setCursor(84, 0);
  if (d < 0) oled.print(" ---cm");
  else       oled.printf("%3.0fcm", d);
}

// ============================================================
//  RELATÓRIO SERIAL (mostra a matemática acontecendo)
// ============================================================
void relatorio(int graus, float d) {
  if (d < 0) {
    Serial.printf("ang=%3d  sem eco\n", graus);
    return;
  }
  float rad = radians(graus);
  float x = d * cos(rad);            // componente horizontal (cm)
  float y = d * sin(rad);            // componente vertical   (cm)

  // Prova real: voltar de cartesiano para polar
  float dVolta   = hypot(x, y);              // sqrt(x² + y²)
  float angVolta = degrees(atan2(y, x));     // ângulo a partir de x e y

  Serial.printf("ang=%3d (%.3f rad) d=%6.1f cm | x=%7.1f y=%6.1f | hypot=%6.1f atan2=%6.1f",
                graus, rad, d, x, y, dVolta, angVolta);

  // tan() = cateto oposto / cateto adjacente (indefinida em 90°)
  if (graus != 90) Serial.printf(" | tan=%.2f", tan(rad));
  Serial.println();
}

// ============================================================
void setup() {
  Serial.begin(115200);

  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  servo.setPeriodHertz(50);            // servos comuns trabalham em 50 Hz
  servo.attach(PINO_SERVO, 500, 2400); // largura de pulso mín/máx em µs

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED nao encontrado!"));
    while (true) delay(10);
  }

  for (int i = 0; i <= 180; i++) distancias[i] = -1;

  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(28, 20);
  oled.print("RADAR");
  oled.display();
  delay(1200);

  Serial.println(F("Radar Ultrassonico - Funcoes Trigonometricas"));
  Serial.println(F("Clique no HC-SR04 na simulacao e mova o controle de distancia."));
}

void loop() {
  servo.write(angulo);
  delay(25);                                   // tempo para o servo chegar na posição

  float d = medirDistanciaCm();

  // Preenche os ângulos do passo atual (evita "buracos" no desenho)
  for (int g = angulo; g != angulo + passo && g >= 0 && g <= 180; g += (passo > 0 ? 1 : -1)) {
    distancias[g] = d;
  }

  oled.clearDisplay();
  desenharGrade();
  desenharEcos();
  desenharFeixe(angulo);
  desenharTexto(angulo, d);
  oled.display();

  relatorio(angulo, d);

  // Avança e inverte o sentido nas extremidades
  angulo += passo;
  if (angulo >= 180 || angulo <= 0) {
    angulo = constrain(angulo, 0, 180);
    passo = -passo;
  }
}
