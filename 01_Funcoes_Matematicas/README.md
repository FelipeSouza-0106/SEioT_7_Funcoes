# Projeto 01 — Estação de Conforto Térmico
**Tópico 7 · Funções predefinidas › Funções matemáticas**
Placa: ESP32 · Duração sugerida: 2 aulas (2 × 50 min)

## Objetivos
Ao final, o aluno deve ser capaz de:
- usar funções matemáticas predefinidas (`pow`, `sqrt`, `sq`, `log`, `abs`, `min`, `max`, `round`, `floor`, `ceil`, `constrain`, `map`) para transformar leituras de sensores em informação útil;
- instalar bibliotecas pelo `lib_deps` do PlatformIO e ler um sensor digital (DHT22);
- exibir dados em um LCD I2C.

**Habilidades do plano de curso:** 1.1, 1.2, 1.3.

## Componentes
| Componente | Novo? | Observação |
|------------|:-----:|------------|
| ESP32 DevKit | | |
| **DHT22** | ✓ | Mede temperatura (−40 a 80 °C) e umidade (0 a 100 %). Saída digital em um único fio. Leitura a cada ≥ 2 s |
| **LCD 16x2 com módulo I2C (PCF8574)** | ✓ | Usa só SDA e SCL. Endereço padrão 0x27 |
| Potenciômetro | | Escolhe a tela exibida |

## Ligações
| Componente | Pino do componente | ESP32 |
|------------|--------------------|-------|
| DHT22 | VCC / SDA (dados) / GND | 3V3 / GPIO 15 / GND |
| LCD I2C | VCC / GND / SDA / SCL | 5V / GND / GPIO 21 / GPIO 22 |
| Potenciômetro | VCC / SIG / GND | 3V3 / GPIO 34 / GND |

## Teoria (exposição — 15 min)
**Função predefinida** é uma rotina pronta, já testada, que vem com a linguagem ou com o framework Arduino. Basta chamá-la pelo nome com os argumentos corretos.

| Função | O que faz | Onde aparece no projeto |
|--------|-----------|-------------------------|
| `pow(base, exp)` | potência | índice de calor (T², UR²) |
| `sq(x)` | x² (atalho) | desvio padrão |
| `sqrt(x)` | raiz quadrada | desvio padrão |
| `log(x)` | logaritmo natural (base *e*) | ponto de orvalho (fórmula de Magnus) |
| `abs(x)` | valor absoluto | diferença entre leitura e média |
| `min(a,b)` / `max(a,b)` | menor / maior | temperatura mínima e máxima |
| `round` / `floor` / `ceil` | arredonda / para baixo / para cima | relatório serial |
| `constrain(x, a, b)` | limita x ao intervalo [a, b] | evita `log(0)`, limita a tela |
| `map(x, a1, b1, a2, b2)` | regra de três entre intervalos (inteiros) | potenciômetro 0–4095 → tela 0–3 |

Pontos de atenção:
- `log()` é logaritmo **natural**. Para base 10 use `log10()`.
- `map()` trabalha com **inteiros** (descarta casas decimais).
- `min`, `max`, `abs`, `sq` e `constrain` são **macros** no Arduino UNO: evite colocar `x++` dentro delas.

## Passo a passo no VS Code
1. **File > Open Folder** → `01_Funcoes_Matematicas`.
2. Mostre o `platformio.ini`: a seção `lib_deps` baixa as bibliotecas DHT e LiquidCrystal_I2C automaticamente.
3. **Build** (✓) → `F1` → **Wokwi: Start Simulator**.
4. Clique no DHT22 durante a simulação e altere temperatura/umidade com os controles deslizantes.
5. Gire o potenciômetro: 4 telas (leitura · sensação/orvalho · mínimo/máximo · média/desvio padrão).

## Experimentos guiados (30 min)
1. Coloque T = 35 °C e UR = 40 %, depois UR = 90 %. O que acontece com a sensação térmica? Por quê?
2. Deixe T = 20 °C e varie a UR. Quando o ponto de orvalho fica igual à temperatura?
3. Faça várias mudanças bruscas de temperatura e observe o desvio padrão na tela 4. O que ele representa?
4. No relatório serial, compare `round`, `floor` e `ceil` para T = 24,5 °C e T = −3,5 °C.

## Desafios
- **Básico:** mostre a temperatura também em Kelvin (K = °C + 273,15) em uma 5ª tela (ajuste o `map` para 0–4).
- **Intermediário:** adicione um LED RGB ao circuito e acenda-o com cor conforme a sensação térmica (azul < 18 °C, verde 18–27 °C, vermelho > 27 °C).
- **Avançado:** calcule a **umidade absoluta** em g/m³:
  `AH = 6,112 · e^(17,67·T/(T+243,5)) · UR · 2,1674 / (273,15 + T)` — use `exp()`.

## Perguntas de verificação
1. Qual a diferença entre `pow(x, 2)` e `sq(x)`?
2. Por que o código usa `constrain(h, 1.0, 100.0)` antes de calcular o ponto de orvalho?
3. Por que `map(2047, 0, 4095, 0, 3)` devolve 1 e não 1,5?
4. Para que serve o operador `%` em `posHist = (posHist + 1) % TAM_HIST`?
