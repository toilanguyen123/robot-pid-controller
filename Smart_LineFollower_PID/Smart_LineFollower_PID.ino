/*
 * Project: AI-Assisted Line Follower Robot with Bluetooth PID Tuning
 * Description: 
 * This system uses a Pololu QTR-8RC sensor array to detect the line position.
 * It applies a Proportional-Integral-Derivative (PID) algorithm to calculate 
 * the necessary motor speed adjustments. 
 * Kp, Ki, Kd parameters can be dynamically updated via Bluetooth.
 */

#include <QTRSensors.h>

// ==========================================
// 1. HARDWARE PIN DEFINITIONS
// ==========================================
// Motor Left Pins (Enable, Phase)
const int motorLeftPWM = 6;
const int motorLeftDir = 9;
// Motor Right Pins (Enable, Phase)
const int motorRightPWM = 3;
const int motorRightDir = 5;

// Sensor Array
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// Buttons & LEDs
const int buttonCalibrate = A3;
const int buttonStart = 2;
const int ledPin = 13;

// ==========================================
// 2. PID & MOTION VARIABLES
// ==========================================
float Kp = 0.05;
float Ki = 0.00001;
float Kd = 0.8;

int lastError = 0;
long integral = 0;

const int baseSpeed = 100;
const int maxSpeed = 150;
const int minSpeed = -50; 

bool isRunning = false;
String btData = ""; // Store incoming Bluetooth commands

// ==========================================
// 3. SETUP 
// ==========================================
void setup() {
  Serial.begin(9600); // Initialize Bluetooth/Serial Communication
  
  // Setup Motor Pins
  pinMode(motorLeftPWM, OUTPUT);
  pinMode(motorLeftDir, OUTPUT);
  pinMode(motorRightPWM, OUTPUT);
  pinMode(motorRightDir, OUTPUT);
  
  pinMode(buttonCalibrate, INPUT_PULLUP);
  pinMode(buttonStart, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Setup QTR Sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){10, 11, 12, A0, A1, A2, A4, A5}, SensorCount);
  qtr.setEmitterPin(7);

  // Wait for user to press Calibrate button
  while(digitalRead(buttonCalibrate) == HIGH) {} 
  calibrateSensors();

  // Wait for user to press Start button
  while(digitalRead(buttonStart) == HIGH) {}
  isRunning = true;
}

// ==========================================
// 4. MAIN LOOP
// ==========================================
void loop() {
  checkBluetoothCommands(); // Check if new PID values are sent via App

  if (isRunning) {
    executePIDControl();
  } else {
    stopMotors();
  }
}

// ==========================================
// 5. CORE FUNCTIONS
// ==========================================

void calibrateSensors() {
  digitalWrite(ledPin, HIGH);
  // Calibrate for ~10 seconds
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  digitalWrite(ledPin, LOW);
}

void executePIDControl() {
  // Read line position (0 to 7000). Ideal center is 3500.
  uint16_t positionLine = qtr.readLineBlack(sensorValues);
  int error = 3500 - positionLine;

  // Calculate PID terms
  int P = error;
  integral += error;
  int D = error - lastError;
  lastError = error; 

  // Final PID Output
  int motorSpeedChange = (P * Kp) + (integral * Ki) + (D * Kd);

  // Adjust individual motor speeds
  int speedLeft = baseSpeed + motorSpeedChange;
  int speedRight = baseSpeed - motorSpeedChange;

  // Constrain speeds to acceptable hardware limits
  speedLeft = constrain(speedLeft, minSpeed, maxSpeed);
  speedRight = constrain(speedRight, minSpeed, maxSpeed);

  driveMotors(speedLeft, speedRight);
}

void driveMotors(int leftSpeed, int rightSpeed) {
  // Left Motor Direction
  if (leftSpeed >= 0) {
    digitalWrite(motorLeftDir, HIGH);
    analogWrite(motorLeftPWM, leftSpeed);
  } else {
    digitalWrite(motorLeftDir, LOW);
    analogWrite(motorLeftPWM, abs(leftSpeed));
  }

  // Right Motor Direction
  if (rightSpeed >= 0) {
    digitalWrite(motorRightDir, HIGH);
    analogWrite(motorRightPWM, rightSpeed);
  } else {
    digitalWrite(motorRightDir, LOW);
    analogWrite(motorRightPWM, abs(rightSpeed));
  }
}

void stopMotors() {
  analogWrite(motorLeftPWM, 0);
  analogWrite(motorRightPWM, 0);
}

// ==========================================
// 6. BLUETOOTH TELEMETRY
// ==========================================
void checkBluetoothCommands() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    btData += inChar;
    
    if (inChar == '\n') {
      parseCommand(btData);
      btData = ""; 
    }
  }
}

void parseCommand(String cmd) {
  cmd.trim(); // Remove whitespace
  
  if (cmd == "STOP") isRunning = false;
  else if (cmd == "START") isRunning = true;
  else if (cmd.startsWith("KP:")) Kp = cmd.substring(3).toFloat();
  else if (cmd.startsWith("KI:")) Ki = cmd.substring(3).toFloat();
  else if (cmd.startsWith("KD:")) Kd = cmd.substring(3).toFloat();
}