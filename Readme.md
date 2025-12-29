# IoT Water Leak Detection System 🚰

**An End-to-End Distributed IoT System based on MCU, Socket Programming, and SQLite.**

## 📖 Overview
This project implements a complete pipeline for detecting water leaks using acoustic signal processing. The system captures analog data from sensors via an **Arduino (ATmega328P)**, processes the signal to filter noise, transmits it via **Serial Communication** to an Edge Gateway (Python), and finally sends the data over a **TCP/IP Socket Network** to a central server for storage and analysis.

The project follows a rigorous engineering roadmap, evolving from hardware simulation in **Proteus** to a fully functional real-time monitoring system.

## 🏗 System Architecture

The system is designed based on a 4-layer architecture:

1.  **Physical Layer (Node):**
    *   **Hardware:** Arduino UNO (ATmega328P) + Acoustic Sensor (Microphone) / Potentiometer (Simulation).
    *   **Firmware Logic:** Signal acquisition with **Moving Average Filter** to reduce jitter.
2.  **Transport Layer (Link):**
    *   Serial Communication (UART) interfacing the MCU with the Edge Gateway.
3.  **Network Layer (Distribution):**
    *   **Python Client:** Reads serial data, appends precise UNIX timestamps, and transmits via TCP Sockets.
    *   **Python Server:** Listens for incoming data packets.
4.  **Application Layer (Backend):**
    *   **Storage:** SQLite Database for historical data persistence.
    *   **Logic:** Threshold-based algorithm to detect potential leaks.

## 🛠 Tech Stack

*   **Microcontroller:** C++ (Arduino IDE / Atmel Studio)
*   **Simulation:** Proteus ISIS (Virtual Prototyping)
*   **Backend & Middleware:** Python 3.x
*   **Libraries:** `pyserial`, `socket`, `sqlite3`
*   **Database:** SQLite

## 📅 7-Day Implementation Roadmap

| Day | Phase | Description | Key Output |
| :--- | :--- | :--- | :--- |
| **01** | **Firmware & Simulation** | Writing MCU code with signal smoothing algorithms. Generating HEX files and verifying logic in Proteus using a Virtual Terminal. | `.hex` file, Proteus Simulation |
| **02** | **Serial Interface** | Developing a Python script to capture raw Serial data and attach real-time timestamps (Data Engineering). | `serial_reader.py` |
| **03** | **Network Architecture** | Decoupling the system into a Client-Server model using TCP Sockets (IoT Standard). | `client.py`, `server.py` |
| **04** | **Data Persistence** | Designing the Database Schema and integrating SQLite for permanent data storage. | `sensor_data.db` |
| **05** | **Logic & Calibration** | Conducting environmental tests to determine the "Noise Floor" and "Leak Threshold". Implementing detection logic. | Calibrated Algorithm |
| **06** | **Reliability & Testing** | Adding error handling (Try-Except blocks) and connection retries to ensure 24/7 stability. | Robust System Code |
| **07** | **Documentation** | Finalizing system diagrams, demo videos, and technical reports. | Final Report |

## ⚙️ Key Technical Features

### 1. Signal Smoothing (Moving Average)
Raw sensor data often contains noise. The firmware implements a moving average filter:
```cpp
// Pseudocode concept
sum = 0;
for(i=0; i<50; i++) sum += analogRead(A0);
output = sum / 50;