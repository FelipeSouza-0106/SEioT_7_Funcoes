/*
 * ============================================================
 *  PROJETO 03 - CENTRAL DE COMANDOS POR TEXTO
 *  Tópico 7 - Funções predefinidas: FUNÇÕES DE TEXTO
 *  Placa: ESP32 DevKit C  |  Simulação: Wokwi (VS Code)
 * ============================================================
 *  Componentes:
 *   - Display LCD 16x2 com módulo I2C        [NOVO]
 *   - LED RGB (cátodo comum) + 3 resistores
 *   - Buzzer
 *
 *  O aluno digita comandos no terminal serial do Wokwi.
 *  O programa "quebra" o texto em partes, interpreta e executa.
 *
 *  Comandos:
 *    ajuda                    -> lista os comandos
 *    cor vermelho|verde|azul|amarelo|branco|desligar
 *    rgb 255,128,0            -> cor personalizada (R,G,B)
 *    msg <texto>              -> mostra no LCD (rola se > 16 letras)
 *    bip <freq> <tempo_ms>    -> toca o buzzer   ex.: bip 440 300
 *    analisa <texto>          -> estatísticas do texto
 *    inverte <texto>          -> escreve o texto de trás para frente
 *
 *  Funções de texto (classe String) exploradas:
 *   length()  trim()  toLowerCase()  toUpperCase()  startsWith()
 *   indexOf()  lastIndexOf()  substring()  charAt()  toInt()
 *   equalsIgnoreCase()  replace()  concat() / operador +
 *   String(numero)  e o padrão de leitura com Serial.read()
 * ============================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ----------------- Pinos -----------------
#define PINO_R      25
#define PINO_G      26
#define PINO_B      27
#define PINO_BUZZER 14

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------- Leitura da serial -------------
String bufferEntrada = "";         // vai juntando os caracteres digitados

// ------------- Letreiro rolante -------------
String textoLetreiro = "";
int    posLetreiro   = 0;
unsigned long ultimoPasso = 0;
const unsigned long VEL_LETREIRO = 350;   // ms por passo

// ============================================================
//  SAÍDAS
// ============================================================
void definirCor(int r, int g, int b) {
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);
  analogWrite(PINO_R, r);
  analogWrite(PINO_G, g);
  analogWrite(PINO_B, b);
  Serial.println("Cor aplicada -> R=" + String(r) + " G=" + String(g) + " B=" + String(b));
}

void escreverLinha(int linha, String texto) {
  // Completa com espaços até 16 caracteres para apagar o que havia antes
  while (texto.length() < 16) texto += ' ';
  lcd.setCursor(0, linha);
  lcd.print(texto.substring(0, 16));
}

void mostrarMensagem(String texto) {
  if (texto.length() <= 16) {
    textoLetreiro = "";                      // não precisa rolar
    escreverLinha(1, texto);
  } else {
    textoLetreiro = texto + "   ";           // espaço entre o fim e o recomeço
    posLetreiro   = 0;
  }
}

// Desloca o texto um caractere por vez (sem travar o loop com delay)
void atualizarLetreiro() {
  if (textoLetreiro.length() == 0) return;
  if (millis() - ultimoPasso < VEL_LETREIRO) return;
  ultimoPasso = millis();

  String duplo = textoLetreiro + textoLetreiro;          // truque para "dar a volta"
  escreverLinha(1, duplo.substring(posLetreiro, posLetreiro + 16));
  posLetreiro = (posLetreiro + 1) % textoLetreiro.length();
}

// ============================================================
//  COMANDOS
// ============================================================
void cmdAjuda() {
  Serial.println(F("Comandos disponiveis:"));
  Serial.println(F("  ajuda"));
  Serial.println(F("  cor vermelho|verde|azul|amarelo|branco|desligar"));
  Serial.println(F("  rgb R,G,B          ex.: rgb 255,128,0"));
  Serial.println(F("  msg <texto>        ex.: msg Ola turma do tecnico!"));
  Serial.println(F("  bip <freq> <ms>    ex.: bip 440 300"));
  Serial.println(F("  analisa <texto>    ex.: analisa Internet das Coisas"));
  Serial.println(F("  inverte <texto>    ex.: inverte arduino"));
}

void cmdCor(String nome) {
  if      (nome.equalsIgnoreCase("vermelho")) definirCor(255, 0, 0);
  else if (nome.equalsIgnoreCase("verde"))    definirCor(0, 255, 0);
  else if (nome.equalsIgnoreCase("azul"))     definirCor(0, 0, 255);
  else if (nome.equalsIgnoreCase("amarelo"))  definirCor(255, 180, 0);
  else if (nome.equalsIgnoreCase("branco"))   definirCor(255, 255, 255);
  else if (nome.equalsIgnoreCase("desligar")) definirCor(0, 0, 0);
  else {
    Serial.println("Cor desconhecida: \"" + nome + "\"");
    return;
  }
  escreverLinha(0, "Cor: " + nome);
}

// "255,128,0" -> três inteiros, usando indexOf() e substring()
void cmdRgb(String valores) {
  int virgula1 = valores.indexOf(',');
  int virgula2 = valores.lastIndexOf(',');

  if (virgula1 < 0 || virgula1 == virgula2) {
    Serial.println(F("Formato invalido. Use: rgb R,G,B"));
    return;
  }
  int r = valores.substring(0, virgula1).toInt();
  int g = valores.substring(virgula1 + 1, virgula2).toInt();
  int b = valores.substring(virgula2 + 1).toInt();

  definirCor(r, g, b);
  escreverLinha(0, "RGB " + String(r) + "," + String(g) + "," + String(b));
}

// "440 300" -> frequência e duração separadas por espaço
void cmdBip(String parametros) {
  int espaco = parametros.indexOf(' ');
  int freq   = parametros.substring(0, espaco).toInt();
  int tempo  = (espaco > 0) ? parametros.substring(espaco + 1).toInt() : 200;

  if (freq <= 0) {
    Serial.println(F("Frequencia invalida. Use: bip 440 300"));
    return;
  }
  tone(PINO_BUZZER, freq, tempo);
  Serial.println("Tocando " + String(freq) + " Hz por " + String(tempo) + " ms");
  escreverLinha(0, "Bip " + String(freq) + "Hz");
}

// Estatísticas sobre um texto: percorre caractere a caractere com charAt()
void cmdAnalisa(String texto) {
  int vogais = 0, consoantes = 0, digitos = 0, espacos = 0;
  String minusculo = texto;
  minusculo.toLowerCase();                       // altera a própria String

  for (unsigned int i = 0; i < minusculo.length(); i++) {
    char c = minusculo.charAt(i);
    if (String("aeiou").indexOf(c) >= 0) vogais++;
    else if (isAlpha(c))                consoantes++;
    else if (isDigit(c))                digitos++;
    else if (c == ' ')                  espacos++;
  }

  String semEspacos = texto;
  semEspacos.replace(" ", "");                  // remove todos os espaços

  String maiusculo = texto;
  maiusculo.toUpperCase();

  Serial.println("Texto ........... \"" + texto + "\"");
  Serial.println("Tamanho ......... " + String(texto.length()) + " caracteres");
  Serial.println("Sem espacos ..... " + semEspacos + " (" + String(semEspacos.length()) + ")");
  Serial.println("MAIUSCULO ....... " + maiusculo);
  Serial.println("minusculo ....... " + minusculo);
  Serial.println("Vogais=" + String(vogais) + "  Consoantes=" + String(consoantes) +
                 "  Digitos=" + String(digitos) + "  Espacos=" + String(espacos));
  int fimPalavra = texto.indexOf(' ');
  if (fimPalavra < 0) fimPalavra = texto.length();     // texto com uma palavra só
  Serial.println("Primeira palavra  " + texto.substring(0, fimPalavra));

  escreverLinha(0, String(texto.length()) + " car " + String(vogais) + " vog");
  mostrarMensagem(maiusculo);
}

void cmdInverte(String texto) {
  String invertido = "";
  for (int i = texto.length() - 1; i >= 0; i--) {
    invertido.concat(texto.charAt(i));          // concat() = adicionar ao final
  }
  Serial.println("Invertido: " + invertido);
  escreverLinha(0, "Invertido:");
  mostrarMensagem(invertido);

  if (invertido.equalsIgnoreCase(texto)) {
    Serial.println(F("E um PALINDROMO!"));
  }
}

// ============================================================
//  INTERPRETADOR: separa "comando" e "argumento"
// ============================================================
void interpretar(String linha) {
  linha.trim();                                  // remove espaços e \r nas pontas
  if (linha.length() == 0) return;

  int espaco = linha.indexOf(' ');
  String comando   = (espaco < 0) ? linha : linha.substring(0, espaco);
  String argumento = (espaco < 0) ? ""    : linha.substring(espaco + 1);
  comando.toLowerCase();
  argumento.trim();

  if      (comando == "ajuda")      cmdAjuda();
  else if (comando == "cor")        cmdCor(argumento);
  else if (comando == "rgb")        cmdRgb(argumento);
  else if (comando == "msg")      { escreverLinha(0, "Mensagem:"); mostrarMensagem(argumento); }
  else if (comando == "bip")        cmdBip(argumento);
  else if (comando == "analisa")    cmdAnalisa(argumento);
  else if (comando == "inverte")    cmdInverte(argumento);
  else if (linha.startsWith("?"))   cmdAjuda();
  else {
    Serial.println("Comando desconhecido: \"" + comando + "\". Digite ajuda.");
  }
}

// ============================================================
void setup() {
  Serial.begin(115200);

  pinMode(PINO_R, OUTPUT);
  pinMode(PINO_G, OUTPUT);
  pinMode(PINO_B, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();
  escreverLinha(0, "Central de Texto");
  escreverLinha(1, "Digite: ajuda");

  Serial.println(F("=== Central de Comandos por Texto ==="));
  cmdAjuda();
}

void loop() {
  // Lê caractere por caractere; ao receber ENTER, interpreta a linha inteira
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (bufferEntrada.length() > 0) {
        Serial.println();                        // fecha a linha do eco
        interpretar(bufferEntrada);
        bufferEntrada = "";
      }
    } else {
      bufferEntrada += c;
      Serial.print(c);                           // eco: mostra o que foi digitado
    }
  }

  atualizarLetreiro();
}
