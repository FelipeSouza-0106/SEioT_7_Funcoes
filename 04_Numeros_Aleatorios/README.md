# Projeto 04 — Jogo da Memória "Genius"
**Tópico 7 · Funções predefinidas › Números aleatórios**
Placa: Arduino UNO · Duração sugerida: 1 a 2 aulas (50 min cada)

## Objetivos
- Gerar números com `random(max)` e `random(min, max)`;
- entender que o microcontrolador gera números **pseudoaleatórios** e que a sequência depende da **semente** (`randomSeed`);
- usar uma fonte imprevisível (o tempo de reação humano, via `micros()`) como semente;
- observar a distribuição dos sorteios com um histograma.

**Habilidades do plano de curso:** 1.1, 1.2, 1.3.

## Componentes
| Componente | Novo? | Observação |
|------------|:-----:|------------|
| Arduino UNO | | |
| 4 LEDs (vermelho, verde, azul, amarelo) + 4 × 220 Ω | | |
| 4 botões | | `INPUT_PULLUP`: solto = HIGH, pressionado = LOW |
| Buzzer | | Uma nota para cada cor |
| LCD 16x2 I2C | ✓ (visto no projeto 1) | No UNO: SDA = A4, SCL = A5 |

## Ligações
| Componente | Arduino UNO |
|------------|-------------|
| LEDs vermelho / verde / azul / amarelo (via 220 Ω) | D9 / D10 / D11 / D12 |
| Botões vermelho / verde / azul / amarelo (outro lado no GND) | D2 / D3 / D4 / D5 |
| Buzzer (+) | D8 |
| LCD I2C SDA / SCL / VCC / GND | A4 / A5 / 5V / GND |

## Teoria (exposição — 15 min)
| Função | O que faz | Exemplo |
|--------|-----------|---------|
| `random(max)` | inteiro de 0 até max − 1 | `random(4)` → 0, 1, 2 ou 3 |
| `random(min, max)` | inteiro de min até max − 1 | `random(1, 7)` → dado de 6 faces |
| `randomSeed(n)` | define o ponto de partida da sequência | `randomSeed(micros())` |

**Pseudoaleatório:** o gerador é uma fórmula. Mesma semente → mesma sequência. Sem `randomSeed`, toda vez que a placa liga os “sorteios” se repetem.

**Fontes de semente:**
- `analogRead()` de um pino desconectado (ruído elétrico) — comum no hardware real, mas **no simulador lê sempre o mesmo valor**;
- **tempo até o usuário apertar um botão** (`micros()`) — funciona no hardware e no simulador. É a técnica usada aqui;
- no ESP32 existe ainda `esp_random()`, um gerador por hardware.

## Passo a passo no VS Code
1. **File > Open Folder** → `04_Numeros_Aleatorios`.
2. **Build** → `F1` → **Wokwi: Start Simulator**.
3. Leia o terminal: primeiro a **demonstração sem semente** e o **histograma**.
4. Aperte qualquer botão para começar. O terminal mostra a semente e o “gabarito” da sequência.
5. Repita a sequência de cores; a cada acerto, ela cresce e fica mais rápida.

## Experimentos guiados (25 min)
1. Reinicie a simulação 3 vezes e anote os 5 números da demonstração sem semente. O que se observa?
2. Compare as sementes e as sequências de duas partidas. Por que agora são diferentes?
3. Troque `randomSeed(semente)` por `randomSeed(42)`. O jogo continua divertido? Que uso prático teria uma semente fixa? (testes reproduzíveis)
4. No histograma, as quatro cores saem exatamente 150 vezes cada? Aumente para 6000 sorteios e compare.
5. Troque `random(0, 5)` dos elogios por `random(0, 4)`. Qual elogio nunca mais aparece? Por quê?

## Desafios
- **Básico:** crie um modo “dado eletrônico”: ao manter o botão amarelo pressionado no início, sorteia `random(1, 7)` e mostra no LCD.
- **Intermediário:** impeça que a mesma cor seja sorteada **três vezes seguidas** na sequência.
- **Avançado:** guarde o recorde na memória EEPROM (`EEPROM.read` / `EEPROM.update`) para não perdê-lo ao desligar — conecta com o tópico 5 (memória física e lógica).

## Perguntas de verificação
1. Quais valores `random(10, 20)` pode devolver?
2. Por que, sem `randomSeed`, o jogo seria sempre igual?
3. Por que no simulador `analogRead(A0)` de um pino solto não é uma boa semente?
4. Como gerar um número decimal entre 0,0 e 1,0 usando `random()`?
