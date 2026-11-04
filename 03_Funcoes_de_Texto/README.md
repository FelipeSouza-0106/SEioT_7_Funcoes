# Projeto 03 — Central de Comandos por Texto
**Tópico 7 · Funções predefinidas › Funções de texto**
Placa: ESP32 · Duração sugerida: 1 a 2 aulas (50 min cada)

## Objetivos
- Manipular textos com os métodos da classe `String` do Arduino;
- receber uma linha de texto pela serial e **interpretá-la** (separar comando e argumentos, converter texto em número);
- controlar saídas (LED RGB, buzzer, LCD) a partir de comandos digitados.

Este padrão — *receber texto, separar, converter e agir* — é exatamente o que se usa depois em Bluetooth, HTTP e MQTT (tópico 8, Conectividade).

**Habilidades do plano de curso:** 1.3, 2.1.

## Componentes
| Componente | Novo? | Observação |
|------------|:-----:|------------|
| ESP32 DevKit | | |
| LCD 16x2 I2C | ✓ (visto no projeto 1) | Mensagens e letreiro rolante |
| LED RGB cátodo comum + 3 × 220 Ω | | Cor via `analogWrite` |
| Buzzer | | `tone(pino, freq, duração)` |

## Ligações
| Componente | Pino do componente | ESP32 |
|------------|--------------------|-------|
| LCD I2C | VCC / GND / SDA / SCL | 5V / GND / GPIO 21 / GPIO 22 |
| LED RGB | R / G / B (via 220 Ω) / COM | GPIO 25 / 26 / 27 / GND |
| Buzzer | + / − | GPIO 14 / GND |

## Teoria (exposição — 15 min)
| Método | Exemplo | Resultado |
|--------|---------|-----------|
| `length()` | `"IoT".length()` | `3` |
| `trim()` | `"  oi \r"` → | `"oi"` (altera a própria String) |
| `toLowerCase()` / `toUpperCase()` | `"Led"` → | `"led"` / `"LED"` (altera a própria String) |
| `indexOf(c)` / `lastIndexOf(c)` | `"255,0,9".indexOf(',')` | `3` (−1 se não achar) |
| `substring(ini, fim)` | `"rgb 255".substring(4)` | `"255"` (o índice `fim` não entra) |
| `charAt(i)` | `"ESP32".charAt(0)` | `'E'` |
| `toInt()` / `toFloat()` | `"440".toInt()` | `440` (0 se não for número) |
| `equalsIgnoreCase(s)` | `"Azul".equalsIgnoreCase("AZUL")` | `true` |
| `startsWith(s)` | `"msg oi".startsWith("msg")` | `true` |
| `replace(a, b)` | `"a b c".replace(" ", "")` | `"abc"` |
| `concat(x)` / `+` | `"T=" + String(25)` | `"T=25"` |

Atenção: `trim()`, `toLowerCase()`, `toUpperCase()` e `replace()` **não devolvem** uma nova String — eles alteram a variável. Erro clássico: `s = s.toLowerCase();` nem compila, porque o método retorna `void`.

## Passo a passo no VS Code
1. **File > Open Folder** → `03_Funcoes_de_Texto`.
2. **Build** → `F1` → **Wokwi: Start Simulator**.
3. Abra o **Serial Monitor do PlatformIO** (ícone da tomada na barra inferior) **com o simulador rodando** e digite os comandos, terminando com **Enter**.

Sequência de teste:
```
ajuda
cor azul
rgb 255,100,0
msg Ola turma do curso tecnico em DS!
bip 880 500
analisa Internet das Coisas 2026
inverte arara
```

## Experimentos guiados (25 min)
1. Digite `COR VERDE` (maiúsculas). Funciona? Qual linha do código garante isso?
2. Digite `rgb 255;0;0`. O que acontece? Como o programa detecta o erro?
3. Digite `rgb 300,-20,abc`. Que valores chegam ao LED? Quem os corrigiu? (`toInt` + `constrain`)
4. Envie uma mensagem com mais de 16 caracteres e explique o “truque” do `duplo = texto + texto` no letreiro.
5. Teste `inverte Socorram me subi no onibus em Marrocos`. Por que não foi detectado como palíndromo? Como corrigir?

## Desafios
- **Básico:** crie o comando `pisca <n>` que pisca o LED RGB n vezes na cor atual.
- **Intermediário:** crie `hex #FF8800` que converte a cor hexadecimal em R, G, B (dica: `strtol(s.c_str(), NULL, 16)`).
- **Avançado:** crie `musica C D E F G` que toca as notas digitadas, separando-as pelos espaços em um laço com `indexOf`.

## Perguntas de verificação
1. O que `indexOf` devolve quando não encontra o caractere procurado?
2. Qual a diferença entre `substring(4)` e `substring(4, 7)`?
3. Por que o programa só interpreta o comando quando chega `'\n'` ou `'\r'`?
4. Por que usar `millis()` no letreiro em vez de `delay()`?

> **Como funciona:** o projeto já está configurado (`rfc2217ServerPort` no `wokwi.toml` e `monitor_port` no `platformio.ini`) para que o Serial Monitor do PlatformIO converse com o Wokwi pela porta 4000.
