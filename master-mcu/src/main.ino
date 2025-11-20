#include <Arduino.h>

// Definições dos pinos
const int buttonOnOff = 4;
const int buttonManual = 10;
const int ledDeviceState = 2;
const int ledManual = 3;

// Pinos para controle do motor (Ponte H)
const int enablePin = 5;   // PWM para velocidade
const int in1 = 6;         // Direção
const int in2 = 7;         // Direção
const int in3 = 8;         // Direção
const int in4 = 9;         // Direção

// Variáveis de estado
bool last_state_manual = false;
bool last_state_device = false;
bool systemActive = false;
bool rotationActive = false;

void setup() {
  // Configuração dos pinos como entrada/saída
  pinMode(ledDeviceState, OUTPUT);
  pinMode(ledManual, OUTPUT);
  pinMode(buttonOnOff, INPUT);
  pinMode(buttonManual, INPUT);
  
  // Configuração dos pinos do motor
  pinMode(enablePin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  process_state_device();
  process_state_motor();
  delay(50); // Debounce
}

void init_rotation(int interval, int potencia = 128) {
  // Ativar motor com velocidade moderada por default
  analogWrite(enablePin, potencia); // 50% da velocidade máxima
   // Estado 1: Pino 6 alto
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(interval);
  Serial.println("Estado 1: Pino 6 alto");
  
  // Estado 2: Pino 7 alto
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(interval);
  Serial.println("Estado 2: Pino 7 alto");
  
  // Estado 3: Pino 8 alto
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(interval);
  Serial.println("Estado 3: Pino 8 alto");
  
  // Estado 4: Pino 9 alto
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(interval);
  Serial.println("Estado 4: Pino 9 alto");
}

void break_rotation() {
  // Desativar motor
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enablePin, 0);
}

int process_state_device() {
  bool currentState = digitalRead(buttonOnOff);
  
  if (currentState == HIGH && !last_state_device) {
    last_state_device = true;
    systemActive = !systemActive;
    
    if (systemActive) {
      digitalWrite(ledDeviceState, HIGH);
      Serial.println("Sistema ativado");
    } else {
      digitalWrite(ledDeviceState, LOW);
      rotationActive = false;
      break_rotation(); // Garante que o motor pare se o sistema for desativado
      Serial.println("Sistema desativado");
    }
  } else if (currentState == LOW) {
    last_state_device = false;
  }
  
  return systemActive ? 1 : 0;
}

int process_state_motor() {
  bool currentState = digitalRead(buttonManual);
  
  if (systemActive) { // Verifica se o sistema está ativo antes de permitir controle do motor
    if (currentState == HIGH && !last_state_manual) {
      last_state_manual = true;
      
      if (!rotationActive) {
        init_rotation(200);
        rotationActive = true;
        digitalWrite(ledManual, HIGH);
        Serial.println("Motor iniciando rotação");
      }
    } else if (currentState == LOW) {
      last_state_manual = false;
      
      if (rotationActive) {
        break_rotation();
        rotationActive = false;
        digitalWrite(ledManual, LOW);
        Serial.println("Motor parado");
      }
    }
  } else {
    // Se o sistema não estiver ativo, garante que o motor está parado
    if (rotationActive) {
      break_rotation();
      rotationActive = false;
      digitalWrite(ledManual, LOW);
    }
  }
  
  return rotationActive ? 1 : 0;
}