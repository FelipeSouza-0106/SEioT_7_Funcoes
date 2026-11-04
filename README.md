# Sistemas Embarcados e IoT — 4º Bimestre
## Tópico 7 das Bases Tecnológicas: Funções Predefinidas

Material de aula com **4 projetos práticos**, um para cada subitem do tópico 7
(o subitem *Bibliotecas* é trabalhado de forma transversal: todos os projetos usam bibliotecas externas declaradas no `platformio.ini`).

| # | Pasta | Subitem | Projeto | Placa | Componente(s) novo(s) |
|---|-------|---------|---------|-------|------------------------|
| 1 | `01_Funcoes_Matematicas` | Funções matemáticas | Estação de Conforto Térmico | ESP32 | DHT22, LCD 16x2 I2C |
| 2 | `02_Funcoes_Trigonometricas` | Funções trigonométricas | Radar Ultrassônico | ESP32 | Servo SG90, HC-SR04, OLED SSD1306 |
| 3 | `03_Funcoes_de_Texto` | Funções de texto | Central de Comandos por Texto | ESP32 | LCD 16x2 I2C (reforço) |
| 4 | `04_Numeros_Aleatorios` | Números aleatórios | Jogo da Memória "Genius" | Arduino UNO | LCD 16x2 I2C (reforço) |

Cada pasta tem um `README.md` com o **roteiro da aula** (objetivos, teoria, montagem, passo a passo, experimentos, desafios e perguntas de verificação).

---

## Como abrir e executar um projeto (VS Code + PlatformIO + Wokwi)

1. No VS Code: **File > Open Folder…** e selecione **a pasta do projeto** (ex.: `01_Funcoes_Matematicas`), e não a pasta `Aulas`.
   O PlatformIO só reconhece o projeto quando o `platformio.ini` está na raiz da pasta aberta.
2. Aguarde o PlatformIO configurar o projeto (na primeira vez ele baixa a plataforma e as bibliotecas — precisa de internet).
3. Compile: ícone **✓ (Build)** na barra inferior, ou `Ctrl+Alt+B`.
4. Simule: `F1` → **Wokwi: Start Simulator**. O arquivo `wokwi.toml` aponta para o firmware gerado em `.pio/build/...`.
5. Abra `diagram.json` para ver o circuito. A saída serial aparece no terminal do Wokwi (no projeto 3 é ali que se digita os comandos).

6. **Monitor serial:** todos os projetos encaminham a serial do simulador para a porta 4000. Com o simulador rodando, o **Serial Monitor do PlatformIO** (ícone da tomada) conecta direto nele, e dá para digitar e enviar texto. Para usar a placa física na USB, comente a linha `monitor_port` do `platformio.ini`.

> Sempre que alterar o código, **compile de novo** antes de reiniciar o simulador — o Wokwi executa o último firmware compilado.

### Estrutura de cada projeto
```
NN_Nome_do_Projeto/
├── platformio.ini        → placa, framework e bibliotecas (lib_deps)
├── wokwi.toml            → onde o Wokwi encontra o firmware compilado
├── diagram.json          → circuito do simulador
├── src/main.cpp          → código-fonte comentado
└── README.md             → roteiro da aula
```

---

## Extensões do VS Code recomendadas

Além de **PlatformIO IDE** e **Wokwi Simulator**:

| Extensão | ID | Para que serve nas aulas |
|----------|----|--------------------------|
| Error Lens | `usernamehw.errorlens` | Mostra o erro de compilação/IntelliSense **na própria linha** do código, em destaque. Ajuda muito o aluno a achar o `;` esquecido |
| Live Share | `ms-vsliveshare.vsliveshare` | O professor entra no VS Code do aluno (ou vice-versa) para ajudar, sem trocar de lugar. Bom para dúvidas e demonstrações |
| Serial Monitor | `ms-vscode.vscode-serial-monitor` | Monitor serial com interface gráfica para as aulas com **placa física** (escolher porta, baud rate, enviar texto) |
| Pacote de idioma Português (Brasil) | `MS-CEINTL.vscode-language-pack-pt-BR` | *Opcional.* Menus em português. Os roteiros citam os menus em inglês (*File > Open Folder*) |

Instalação de uma vez (PowerShell ou terminal do VS Code):
```powershell
$code = "C:\Program Files\Microsoft VS Code\bin\code.cmd"
"usernamehw.errorlens","ms-vsliveshare.vsliveshare","ms-vscode.vscode-serial-monitor" | ForEach-Object { & $code --install-extension $_ }
```

---

## Sugestão de cronograma

| Encontro | Conteúdo |
|----------|----------|
| 1 | Introdução ao tópico 7 · o que é uma função predefinida · bibliotecas e `lib_deps` · Projeto 1 (parte 1: DHT22 + LCD I2C, conversões, `pow`, `log`) |
| 2 | Projeto 1 (parte 2: estatística com `min`, `max`, `sq`, `sqrt`, `abs`) + desafios |
| 3 | Revisão de trigonometria (ciclo trigonométrico, graus × radianos) · Projeto 2 |
| 4 | Projeto 2: experimentos com `atan2` e `hypot` + desafios |
| 5 | Projeto 3: classe `String`, interpretador de comandos |
| 6 | Projeto 4: pseudoaleatoriedade, `randomSeed`, jogo Genius |
| 7 | Desafio integrador / avaliação prática (ver sugestão abaixo) |

**Desafio integrador sugerido:** “Dado de RPG IoT” — o usuário digita `rolar 3d6+2` no terminal (funções de texto), o sistema sorteia os dados (números aleatórios), mostra soma, média e desvio padrão (funções matemáticas) e desenha um ponteiro girando no OLED (funções trigonométricas).

---

## Componentes comuns em kits básicos Arduino/ESP32 (sugestões para as próximas aulas)

Itens já conhecidos pela turma: resistor, LED, LED RGB, botão, LDR, buzzer, potenciômetro.

| Componente | Para que serve | Usado aqui? | Disponível no Wokwi |
|------------|----------------|:-----------:|:-------------------:|
| Display LCD 16x2 com módulo I2C | Exibir texto com só 2 fios de dados | Projetos 1, 3, 4 | ✓ |
| Display OLED 0,96" SSD1306 | Gráficos e texto em 128x64 pixels | Projeto 2 | ✓ |
| Sensor DHT11 / DHT22 | Temperatura e umidade | Projeto 1 | ✓ |
| Sensor ultrassônico HC-SR04 | Medir distância (2 a 400 cm) | Projeto 2 | ✓ |
| Micro servo SG90 | Posicionar um eixo de 0° a 180° | Projeto 2 | ✓ |
| Display de 7 segmentos / módulo TM1637 (4 dígitos) | Mostrar números | — | ✓ |
| Matriz de LED 8x8 com MAX7219 | Ícones, animações, letreiros | — | ✓ |
| Teclado matricial de membrana 4x4 | Entrada de senhas e números | — | ✓ |
| Módulo joystick analógico | Dois potenciômetros + botão (eixos X/Y) | — | ✓ |
| Sensor de movimento PIR HC-SR501 | Detectar presença | — | ✓ |
| Módulo relé 5V | Ligar cargas (lâmpadas, ventiladores) | — | ✓ |
| Motor de passo 28BYJ-48 + driver ULN2003 | Movimento preciso por passos | — | ✓ (stepper) |
| Módulo RTC DS1307 / DS3231 | Relógio de tempo real | — | ✓ |
| Acelerômetro/giroscópio MPU6050 | Inclinação e movimento | — | ✓ |
| Receptor infravermelho + controle remoto | Comando sem fio | — | ✓ |
| Fita/anel de LEDs WS2812 (NeoPixel) | LEDs RGB endereçáveis | — | ✓ |
| Motor DC + ponte H L298N / L9110 | Controle de velocidade e sentido | — | parcial |
| Sensor de umidade do solo, chama, som (KY-0xx) | Saídas analógica/digital simples | — | parcial |
| Módulo leitor RFID RC522 | Leitura de cartões/tags | — | — |

Boa conexão com o próximo tópico (8 – Conectividade): o ESP32 com Wi-Fi + DHT22 do projeto 1 vira facilmente um **servidor HTTP embarcado** com página de monitoramento.

---

## Observações para o hardware real

- **ESP32 é 3,3 V.** O pino ECHO do HC-SR04 sai 5 V: use divisor resistivo (ex.: 1 kΩ + 2 kΩ) antes do GPIO 19. No simulador não é necessário.
- **Servo** puxa picos de corrente: em bancada, prefira alimentação 5 V externa com GND comum.
- **LCD I2C:** o endereço pode ser `0x27` ou `0x3F` dependendo do módulo; ajuste o contraste no trimpot azul do módulo.
- O Wokwi e o PlatformIO precisam de internet na primeira compilação (download da plataforma e das bibliotecas).
