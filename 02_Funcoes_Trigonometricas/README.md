# Projeto 02 — Radar Ultrassônico
**Tópico 7 · Funções predefinidas › Funções trigonométricas**
Placa: ESP32 · Duração sugerida: 2 aulas (2 × 50 min)

## Objetivos
- Converter coordenadas **polares** (distância + ângulo) em **cartesianas** (x, y) com `sin()` e `cos()`;
- usar `radians()` / `degrees()` e entender por que as funções trigonométricas trabalham em radianos;
- fazer a conversão inversa com `atan2()` e `hypot()`;
- operar três componentes novos: servo, sensor ultrassônico e display OLED.

**Habilidades do plano de curso:** 1.1, 1.2, 1.3.

## Componentes
| Componente | Novo? | Observação |
|------------|:-----:|------------|
| ESP32 DevKit | | |
| **Micro servo SG90** | ✓ | Gira de 0° a 180° conforme a largura do pulso PWM (≈ 0,5 a 2,4 ms, 50 Hz) |
| **Sensor ultrassônico HC-SR04** | ✓ | Emite um pulso de 40 kHz e mede o tempo do eco. `d = tempo × 0,0343 / 2` |
| **OLED 0,96" SSD1306 (I2C)** | ✓ | 128 × 64 pixels, endereço 0x3C. Origem (0,0) no canto superior esquerdo |

## Ligações
| Componente | Pino do componente | ESP32 |
|------------|--------------------|-------|
| Servo | V+ / GND / PWM | 5V / GND / GPIO 18 |
| HC-SR04 | VCC / TRIG / ECHO / GND | 5V / GPIO 5 / GPIO 19 / GND |
| OLED | VCC / GND / SDA / SCL | 3V3 / GND / GPIO 21 / GPIO 22 |

## Teoria (exposição — 20 min)
Desenhe no quadro um semicírculo com o sensor no centro. Um objeto a **d** cm, no ângulo **θ**, está em:

```
x = d · cos(θ)        y = d · sin(θ)
d = √(x² + y²)        θ = atan2(y, x)
```

| Função | O que faz | Uso no projeto |
|--------|-----------|----------------|
| `radians(graus)` | graus → radianos (× π/180) | antes de todo `sin`/`cos` |
| `degrees(rad)` | radianos → graus | resultado do `atan2` |
| `sin(rad)` / `cos(rad)` | seno / cosseno | posição na tela e x, y em cm |
| `tan(rad)` | tangente (= sin/cos) | relatório serial (indefinida em 90°) |
| `atan2(y, x)` | ângulo do ponto (x, y), em radianos, considerando o quadrante | prova real |
| `hypot(x, y)` | √(x² + y²) | prova real |
| `PI` | 3,14159… | constante pronta |

**Detalhe importante:** no display, o eixo **y cresce para baixo**. Por isso o código usa `CENTRO_Y - raio * sin(θ)`.

Pergunta provocadora: *por que existe `atan2(y, x)` se já existe `atan(y/x)`?* (Resposta: `atan` não distingue quadrantes e quebra quando x = 0.)

## Passo a passo no VS Code
1. **File > Open Folder** → `02_Funcoes_Trigonometricas`.
2. **Build** → `F1` → **Wokwi: Start Simulator**.
3. Observe o feixe varrendo o OLED e o relatório no terminal.
4. Clique no HC-SR04 e mova o controle de distância **enquanto o servo gira**: o radar “desenha” o objeto.

## Experimentos guiados (30 min)
1. Fixe a distância em 100 cm. Que figura aparece no OLED? Por quê?
2. Altere a distância várias vezes durante uma varredura para desenhar uma “parede” e um “objeto próximo”.
3. No relatório serial, confira se `hypot` devolve a mesma distância medida e se `atan2` devolve o mesmo ângulo. Onde há pequenas diferenças? (arredondamento de ponto flutuante)
4. O que acontece com `tan()` perto de 90°? Mostre no serial.
5. Troque `radians(graus)` por apenas `graus` em `polarParaTela`. O que acontece com o desenho?

## Desafios
- **Básico:** mude o alcance (`ALCANCE_CM`) para 400 cm e a varredura para passos de 1°.
- **Intermediário:** acenda um LED vermelho e toque o buzzer quando houver objeto a menos de 30 cm, mostrando no serial as coordenadas (x, y) dele.
- **Avançado:** adicione um **joystick** (eixos X/Y em GPIO 34 e 35). Calcule o ângulo com `atan2` e a intensidade com `hypot`, e faça o servo apontar para a direção do joystick.

## Perguntas de verificação
1. Quanto vale 90° em radianos?
2. Se o objeto está em x = −50 cm, y = 50 cm, qual é o ângulo? E a distância?
3. Por que o código divide o tempo do eco por 2?
4. Por que no OLED se usa `CENTRO_Y - ...` e não `CENTRO_Y + ...`?

## Hardware real
O ECHO do HC-SR04 sai em 5 V e o ESP32 aceita 3,3 V: use um divisor resistivo (1 kΩ + 2 kΩ). Alimente o servo com fonte 5 V externa e GND comum.
