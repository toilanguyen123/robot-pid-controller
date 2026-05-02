# 🤖 Line Follower Robot — AI-Assisted PID Development

> An Arduino-based line follower robot featuring real-time PID control, QTR-8RC sensor array, and live Bluetooth parameter tuning via HC-05. Built as a hands-on robotics engineering learning project with AI as a development partner.

---

## 📌 Project Overview

This project implements a **PID-controlled line follower robot** on Arduino Uno. The robot reads line position from a **Pololu QTR-8RC** 8-sensor array, calculates correction using a PID algorithm, and drives two DC motors via an **L298N** driver module. PID parameters (Kp, Ki, Kd) can be updated **live over Bluetooth** without re-flashing the board.

**Core problem solved:** Eliminating the traditional re-flash cycle when tuning PID — every parameter change previously required a cable reconnect and code upload. The Bluetooth live-tuning system reduces iteration time drastically.

---

## ✨ Features

- **PID Control Loop** — Proportional, Integral, Derivative control for smooth line tracking
- **QTR-8RC Sensor Integration** — 8-channel IR reflectance sensor with built-in calibration routine
- **Live Bluetooth Tuning** — Send `KP:0.05`, `KI:0.00001`, `KD:0.8` from any Bluetooth terminal app to update parameters on the fly
- **Non-blocking Serial Read** — Custom `checkBluetoothCommands()` using character-by-character reading — prevents the 1-second freeze caused by `Serial.readString()`
- **Motor Direction Handling** — Correct `abs()` + direction pin logic for negative speed (reverse) with L298N
- **START/STOP Control** — Hardware button or Bluetooth `START`/`STOP` commands

---

## 🧠 AI-Assisted Development

This project was built using an **AI-in-the-loop engineering workflow**:

| Stage | AI Contribution |
|-------|----------------|
| **Concept** | Explained PID theory (P/I/D physical meaning) in the context of a QTR sensor range of 0–7000 |
| **Code Generation** | Generated `executePIDControl()` with correct L298N negative-speed handling using `abs()` |
| **Debugging** | Diagnosed overshoot (Kp=0.2 causing `P = 400` at error=2000, exceeding maxSpeed) and recommended parameter ranges |
| **Architecture** | Designed non-blocking Bluetooth read using character buffering to avoid freezing the PID loop |
| **Iteration** | Refined `parseCommand()` and `driveMotors()` based on hardware feedback |

The AI acted as a **real-time engineering mentor** — not just generating code, but explaining *why* each design decision matters physically.

---

## 🔧 Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | Arduino Uno |
| Sensor Array | Pololu QTR-8RC (8 channels) |
| Motor Driver | L298N Dual H-Bridge |
| Bluetooth Module | HC-05 (connected to Serial RX/TX) |
| Motors | 2x DC motors (left & right) |
| Buttons | Calibrate (A3), Start (D2) |
| LED | Status indicator (D13) |

### Pin Mapping

```
Motor Left  → PWM: D6,  DIR: D9
Motor Right → PWM: D3,  DIR: D5
Sensors     → D10, D11, D12, A0, A1, A2, A4, A5
Emitter Pin → D7
Bluetooth   → RX/TX (D0/D1)
```

---

## 🚀 Getting Started

### 1. Install Dependencies
In Arduino IDE → Library Manager, install:
- `QTRSensors` by Pololu

### 2. Upload Code
> ⚠️ **Important:** Disconnect HC-05 TX/RX wires before uploading. Arduino Uno shares Serial hardware with USB.

### 3. Calibration
1. Power on → Press **Calibrate button** (A3)
2. Move the robot slowly across the line for ~10 seconds
3. LED turns off when calibration is complete

### 4. Start
Press **Start button** (D2) — robot begins PID loop.

### 5. Bluetooth Tuning
Connect HC-05 via any Bluetooth Terminal app. Send commands:
```
KP:0.05       → Set Kp
KI:0.00001    → Set Ki  
KD:0.8        → Set Kd
STOP          → Stop motors
START         → Resume
```

---

## 📐 PID Algorithm

```
error = 3500 - sensorPosition   // 3500 = center setpoint

P = Kp × error
I = Ki × Σ(error)
D = Kd × (error - lastError)

motorSpeedChange = P + I + D

speedLeft  = baseSpeed + motorSpeedChange
speedRight = baseSpeed - motorSpeedChange
```

**Default tuned values:** `Kp=0.05`, `Ki=0.00001`, `Kd=0.8`

---

## 📁 File Structure

```
robot-pid-controller/
├── README.md
└── line_follower_pid_bt.ino   ← Main Arduino sketch
```

---

## 👤 Author

**Tran Khoi Nguyen**  
Robotics Engineering Student — Vietnam National University, Hanoi (VNU-UET)  
Built as part of self-directed robotics curriculum using AI-assisted development methodology.
