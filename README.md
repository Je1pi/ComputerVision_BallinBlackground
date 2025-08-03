# Ball-and-Plate: Sistema de Equilibrio de Bola

Este projeto implementa um sistema de controle ball-and-plate (bola em mesa móvel) que utiliza visão computacional e controle PID para manter uma bola equilibrada no centro de uma mesa móvel.

## 📖 Sobre o Projeto

Este é um sistema completo que combina:

1. **Sistema de Visão (PC):** Um script em Python, utilizando a biblioteca OpenCV, é responsável por capturar o vídeo de uma webcam, processar os frames em tempo real para detectar a posição `(x, y)` de uma bola, e enviar essas coordenadas via comunicação serial (USB).

2. **Sistema de Controle (Arduino):** Recebe as coordenadas da bola e utiliza controle PID para ajustar dois servo motores, movimentando a mesa para manter a bola equilibrada no centro.

## 📋 Componentes Necessários

### Hardware:
- 1x Arduino Uno/Nano
- 2x Servo motores (SG90 ou similar)
- 1x Mesa/plataforma móvel conectada aos servos
- 1x Webcam
- 1x Bola (preferencialmente branca)
- Cabos e conectores

### Software:
- Python 3.x
- OpenCV (cv2)
- PySerial
- Arduino IDE

## 🔧 Instalação

### 1. Dependências Python:
```bash
pip install opencv-python pyserial numpy
```

### 2. Configuração do Arduino:
1. Conecte os servos aos pinos 9 (eixo X) e 10 (eixo Y) do Arduino
2. Carregue o código `Arduino/ball-and-plate.ino` no Arduino
3. Verifique a porta serial (geralmente `/dev/ttyACM0` ou `/dev/ttyUSB0` no Linux)

## 🚀 Como Usar

1. **Conecte o Arduino** via USB e verifique a porta serial
2. **Calibre a cor da bola:**
   ```bash
   python calibrador_hsv.py
   ```
   - Use os controles deslizantes para ajustar até que apenas a bola apareça branca
   - Anote os valores e atualize `LOWER_WHITE` e `UPPER_WHITE` no `webcam_teste.py`

3. **Execute o sistema:**
   ```bash
   python webcam_teste.py
   ```
4. **Posicione a bola** na mesa e observe o sistema tentar equilibrá-la no centro

## ⚙️ Configurações

### Parâmetros do PID (Arduino):
- `kp_x`, `kp_y`: Ganho proporcional (padrão: 0.8)
- `ki_x`, `ki_y`: Ganho integral (padrão: 0.01) 
- `kd_x`, `kd_y`: Ganho derivativo (padrão: 0.3)

### Comandos via Serial Monitor:
- `kp[valor]`: Ajusta ganho proporcional
- `ki[valor]`: Ajusta ganho integral  
- `kd[valor]`: Ajusta ganho derivativo
- `reset`: Reseta sistema para posição central
- `help`: Mostra comandos disponíveis

## 📊 Como Funciona o Processamento de Imagem

### Pré-processamento da Máscara:
```python
kernel = np.ones((5, 5), np.uint8)                        # Cria um kernel 5x5 de uns
mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)    # Expanse a area branca para tampar buracos
mask = cv2.erode(mask, None, iterations=1)                # Encolhe um pouco as áreas brancas
mask = cv2.dilate(mask, None, iterations=2)               # Expande as áreas brancas
```

**Explicação:**
- **Kernel 5x5:** Representa uma espécie de pincel para "scanear" a imagem
- **MORPH_CLOSE:** Expande as áreas brancas cobrindo pequenos buracos pretos
- **Erode:** Encolhe as áreas brancas eliminando ruídos pequenos
- **Dilate:** Expande novamente para destacar a bola, mantendo apenas objetos grandes

## 🔍 Detecção de Problemas

### Arduino não conecta:
**Descobrir porta COM:**
```bash
python -m serial.tools.list_ports
```

**Portas comuns no Linux:**
- `/dev/ttyACM0` (mais comum)
- `/dev/ttyUSB0` (alternativa)

**Dar permissões (se necessário):**
```bash
sudo chmod 666 /dev/ttyACM0
```

### Bola não é detectada:
1. Use o `calibrador_hsv.py` para recalibrar as cores
2. Verifique iluminação do ambiente
3. Ajuste os parâmetros de tamanho da bola no código
4. Certifique-se que a bola contrasta bem com o fundo

### Sistema instável:
1. Ajuste os parâmetros PID via Serial Monitor
2. Verifique se os servos estão bem fixados
3. Reduza os valores de `MAX_ANGLE_CHANGE` no Arduino
4. Ajuste o filtro de suavização (`alpha`)

## 📁 Estrutura do Projeto

```
/
├── webcam_teste.py          # Detecção da bola via webcam
├── calibrador_hsv.py        # Ferramenta de calibração de cores
├── teste_sistema.py         # Teste do sistema completo
├── README.md                # Este arquivo
└── Arduino/
    └── ball-and-plate.ino   # Controle PID e servos
```

## 🔬 Fluxo do Sistema

1. **Visão Computacional**: A webcam detecta a posição da bola usando segmentação por cor HSV
2. **Comunicação Serial**: As coordenadas (x,y) são enviadas via USB para o Arduino
3. **Controle PID**: O Arduino calcula os erros e ajusta os servos para corrigir a posição
4. **Atuação**: Os servos movem a mesa para trazer a bola de volta ao centro

## 📊 Parâmetros de Ajuste Fino

- **Ganho Proporcional (Kp)**: Controla resposta imediata ao erro
- **Ganho Integral (Ki)**: Corrige erros acumulados ao longo do tempo  
- **Ganho Derivativo (Kd)**: Prediz tendência futura e reduz oscilações
- **Filtro Alpha**: Suaviza movimentos bruscos da detecção (0.7 = 70% novo, 30% anterior)

**Dica:** Ajuste gradualmente estes valores observando o comportamento do sistema!

## 🐛 Dependências por Sistema

### Ubuntu/Debian:
```bash
sudo apt install python3-pip
pip3 install opencv-python pyserial numpy
```

### Arch Linux:
```bash
sudo pacman -S python-opencv python-pyserial python-numpy
```

### Geral (pip):
```bash
pip install opencv-python pyserial numpy
```

