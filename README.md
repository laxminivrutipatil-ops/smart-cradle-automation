# smart-cradle-automation
An automated child healthcare embedded solution built on an ESP32 microcontroller using C++ and the Arduino IDE framework to track real-time moisture variables and maintain infant safety.
# Smart Cradle Automation System

An automated child healthcare IoT solution designed to actively monitor ambient cradle environment variables to keep infants dry, safe, and comfortable. This project secured **1st Rank** across the annual college-level technical project exhibition during my 1st semester.

## Core Features
* **Automated Wetness Tracking:** Uses an analog moisture input mapped across the ESP32 ADC to catch dampness patterns instantly.
* **Audio Warning Stream:** Integrates hardware communication protocols to broadcast a pre-recorded vocal sound snippet once moisture thresholds breach safety targets.
* **Manual Swing Control:** Employs hardware debounced external interrupts (`IRAM_ATTR`) combined with a physical mechanical switch to allow mothers manual override control of the cradle rocking motor at any time.

## Tech Stack
* **Microcontroller:** ESP32 Module
* **Firmware Framework:** Arduino IDE Architecture
* **Core Language:** C++
* **Sensory Interfacing:** Moisture Sensors, Analog-to-Digital Processing (ADC)
