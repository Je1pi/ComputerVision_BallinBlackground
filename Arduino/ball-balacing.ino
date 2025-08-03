#include <Servo.h>

// Definição dos servos
Servo servoX;
Servo servoY;

// Pinos dos servos
const int SERVO_X_PIN = 9;
const int SERVO_Y_PIN = 10;

// Configurações da mesa/câmera
const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;
const int CENTER_X = CAMERA_WIDTH / 2;
const int CENTER_Y = CAMERA_HEIGHT / 2;

// Configurações dos servos (em graus)
const int SERVO_CENTER = 90;
const int SERVO_MIN = 30;
const int SERVO_MAX = 150;
const int MAX_ANGLE_CHANGE = 20;

// Variáveis para posição da bola
int ballX = CENTER_X;
int ballY = CENTER_Y;
bool ballDetected = false;
unsigned long lastBallTime = 0;
const unsigned long BALL_TIMEOUT = 1000;

// Variáveis do controlador PID para eixo X
float kp_x = 0.8;
float ki_x = 0.01;
float kd_x = 0.3;
float error_x = 0;
float last_error_x = 0;
float integral_x = 0;
float derivative_x = 0;
float output_x = 0;

// Variáveis do controlador PID para eixo Y
float kp_y = 0.8;
float ki_y = 0.01;
float kd_y = 0.3;
float error_y = 0;
float last_error_y = 0;
float integral_y = 0;
float derivative_y = 0;
float output_y = 0;

// Posições atuais dos servos
int currentServoX = SERVO_CENTER;
int currentServoY = SERVO_CENTER;

// Filtro para suavizar movimentos
float alpha = 0.7;
float filteredX = CENTER_X;
float filteredY = CENTER_Y;

// Variáveis de timing
unsigned long lastPIDTime = 0;
const int PID_INTERVAL = 50;

void setup() {
  Serial.begin(9600);
  
  // Inicializa os servos
  servoX.attach(SERVO_X_PIN);
  servoY.attach(SERVO_Y_PIN);
  
  // Posiciona os servos no centro
  servoX.write(SERVO_CENTER);
  servoY.write(SERVO_CENTER);
  
  Serial.println("Sistema Ball-and-Plate inicializado!");
  Serial.println("Aguardando dados da câmera...");
  
  delay(1000);
}

void loop() {
  readSerialData();
  
  if (millis() - lastPIDTime >= PID_INTERVAL) {
    if (ballDetected && (millis() - lastBallTime < BALL_TIMEOUT)) {
      filteredX = alpha * ballX + (1 - alpha) * filteredX;
      filteredY = alpha * ballY + (1 - alpha) * filteredY;
      
      runPIDControl();
      
      updateServos();
      
      printDebugInfo();
    } else {
      returnToCenter();
    }
    
    lastPIDTime = millis();
  }
  
  delay(10);
}

void readSerialData() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    data.trim();
    
    // Parse das coordenadas "x,y"
    int commaIndex = data.indexOf(',');
    if (commaIndex > 0) {
      ballX = data.substring(0, commaIndex).toInt();
      ballY = data.substring(commaIndex + 1).toInt();
      
      // Valida se as coordenadas estão dentro dos limites
      if (ballX >= 0 && ballX <= CAMERA_WIDTH && 
          ballY >= 0 && ballY <= CAMERA_HEIGHT) {
        ballDetected = true;
        lastBallTime = millis();
      }
    }
  }
}

void runPIDControl() {
  // Calcula erro (diferença entre posição atual e centro)
  error_x = CENTER_X - filteredX;
  error_y = CENTER_Y - filteredY;
  
  // Componente integral (acumula erro ao longo do tempo)
  integral_x += error_x;
  integral_y += error_y;
  
  // Limita o componente integral para evitar windup
  integral_x = constrain(integral_x, -1000, 1000);
  integral_y = constrain(integral_y, -1000, 1000);
  
  // Componente derivativo (taxa de mudança do erro)
  derivative_x = error_x - last_error_x;
  derivative_y = error_y - last_error_y;
  
  // Calcula saída PID
  output_x = (kp_x * error_x) + (ki_x * integral_x) + (kd_x * derivative_x);
  output_y = (kp_y * error_y) + (ki_y * integral_y) + (kd_y * derivative_y);
  
  // Salva erro atual para próxima iteração
  last_error_x = error_x;
  last_error_y = error_y;
}

void updateServos() {
  // Converte saída PID para ângulo do servo
  int targetAngleX = SERVO_CENTER + constrain(output_x * 0.1, -MAX_ANGLE_CHANGE, MAX_ANGLE_CHANGE);
  int targetAngleY = SERVO_CENTER + constrain(output_y * 0.1, -MAX_ANGLE_CHANGE, MAX_ANGLE_CHANGE);
  
  // Limita ângulos aos limites dos servos
  targetAngleX = constrain(targetAngleX, SERVO_MIN, SERVO_MAX);
  targetAngleY = constrain(targetAngleY, SERVO_MIN, SERVO_MAX);
  
  // Suaviza movimento dos servos
  if (abs(targetAngleX - currentServoX) > 1) {
    if (targetAngleX > currentServoX) currentServoX++;
    else currentServoX--;
  }
  
  if (abs(targetAngleY - currentServoY) > 1) {
    if (targetAngleY > currentServoY) currentServoY++;
    else currentServoY--;
  }
  
  // Atualiza posições dos servos
  servoX.write(currentServoX);
  servoY.write(currentServoY);
}

void returnToCenter() {
  // Retorna gradualmente ao centro quando a bola não é detectada
  if (currentServoX > SERVO_CENTER) {
    currentServoX--;
  } else if (currentServoX < SERVO_CENTER) {
    currentServoX++;
  }
  
  if (currentServoY > SERVO_CENTER) {
    currentServoY--;
  } else if (currentServoY < SERVO_CENTER) {
    currentServoY++;
  }
  
  servoX.write(currentServoX);
  servoY.write(currentServoY);
  
  integral_x = 0;
  integral_y = 0;
  last_error_x = 0;
  last_error_y = 0;
}

void printDebugInfo() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.print("Bola: (");
    Serial.print(ballX);
    Serial.print(", ");
    Serial.print(ballY);
    Serial.print(") | Erro: (");
    Serial.print(error_x, 1);
    Serial.print(", ");
    Serial.print(error_y, 1);
    Serial.print(") | Servos: (");
    Serial.print(currentServoX);
    Serial.print(", ");
    Serial.print(currentServoY);
    Serial.println(")");
    
    lastPrint = millis();
  }
}

// Função para ajustar parâmetros PID via Serial Monitor
void adjustPIDParameters() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command.startsWith("kp")) {
      float value = command.substring(2).toFloat();
      kp_x = kp_y = value;
      Serial.print("Kp ajustado para: ");
      Serial.println(value);
    }
    else if (command.startsWith("ki")) {
      float value = command.substring(2).toFloat();
      ki_x = ki_y = value;
      Serial.print("Ki ajustado para: ");
      Serial.println(value);
    }
    else if (command.startsWith("kd")) {
      float value = command.substring(2).toFloat();
      kd_x = kd_y = value;
      Serial.print("Kd ajustado para: ");
      Serial.println(value);
    }
    else if (command == "reset") {
      currentServoX = SERVO_CENTER;
      currentServoY = SERVO_CENTER;
      servoX.write(SERVO_CENTER);
      servoY.write(SERVO_CENTER);
      integral_x = integral_y = 0;
      last_error_x = last_error_y = 0;
      Serial.println("Sistema resetado para posição central");
    }
    else if (command == "help") {
      Serial.println("Comandos disponíveis:");
      Serial.println("kp[valor] - Ajusta ganho proporcional");
      Serial.println("ki[valor] - Ajusta ganho integral");
      Serial.println("kd[valor] - Ajusta ganho derivativo");
      Serial.println("reset - Reseta sistema para posição central");
      Serial.println("help - Mostra esta ajuda");
    }
  }
}
