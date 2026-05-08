# ModuBot-Robotic-System

Modular multi-mode robotic platform built using Arduino and ESP32 for autonomous navigation, patrol behavior, embedded systems experimentation, and robotics architecture development.

---

## Overview

ModuBot is a multifunctional robotics platform designed to explore real-world robotics concepts including:

- Autonomous obstacle avoidance
- Manual remote-controlled operation
- Route recording and playback
- Patrol navigation systems
- Embedded sensor integration
- Modular robotics software architecture
- Behavioral state machines (FSM)
- Autonomous decision-making systems

The project evolved from a simple Arduino-based robot into a more scalable ESP32 robotics platform focused on cleaner architecture, modular subsystem design, and future autonomous capabilities.

---

## Core Features

- Multi-mode robotic control system
- IR remote manual driving
- Autonomous obstacle avoidance
- Patrol route playback
- Route logging and replay
- Obstacle interruption and resume logic
- Ultrasonic sensor scanning system
- Servo-based environmental scanning
- FSM-based behavior architecture
- Modular firmware organization
- Embedded systems experimentation platform

---

## Firmware Platforms

### Arduino Version

The original implementation of ModuBot was developed on Arduino and served as the foundation for the project architecture and behavior systems.

### ESP32 Version

The project was later migrated to ESP32 to support:

- Improved scalability
- Expanded peripheral support
- Better multitasking capability
- Cleaner modular architecture
- Future autonomous robotics features
- More advanced embedded systems experimentation

Development is now primarily focused on the ESP32 implementation.

---

## Arduino Refactor Status

The Arduino firmware refactor was paused after the project was successfully ported to ESP32.

Since active subsystem testing and architectural improvements are now being performed on the ESP32 platform, development efforts were consolidated there rather than maintaining two parallel firmware architectures.

The complete Arduino implementation remains included in this repository for reference and historical development tracking.

---

## Current Architecture Goals

The ongoing ESP32 refactor focuses on transitioning ModuBot toward a cleaner robotics software architecture featuring:

- Fully modular subsystem separation
- Non-blocking FSM-based behavior control
- Dedicated sensor abstraction layers
- Scanning and navigation systems
- Route memory and playback systems
- Autonomous behavior arbitration
- Expandable robotics framework design

---

## Technologies Used

- Arduino C/C++
- ESP32
- Ultrasonic Sensors
- Servo Motors
- IR Remote Systems
- H-Bridge Motor Drivers
- PWM Motor Control
- Embedded Finite State Machines (FSM)
- Fusion 360 (mechanical design)

---

## Future Development

Planned future improvements include:

- ESP32 dual-core task architecture
- Sensor fusion systems
- MPU6050 integration
- OLED telemetry display
- PCB integration
- Wireless telemetry
- Autonomous mapping experiments
- Improved robotics navigation logic
- Advanced modular attachments

---

## Project Purpose

ModuBot serves as both:

- A practical robotics experimentation platform
- A long-term embedded systems and robotics engineering learning project

The project emphasizes architecture, modularity, autonomous behavior systems, and iterative engineering development rather than simple feature accumulation.
