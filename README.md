# Senior Design — Cabin Environmental Monitoring System

UCF ECE Senior Design (EEL 4914/4915L), Fall 2026 – Spring 2027.

## Team

| Name | Role |
|---|---|
| Orion Brown | Computer Engineer |
| Matias Guillen | Electrical Engineer |
| Sebastian Candelaria | Computer Engineer |
| Kevin | Computer Engineer |

**Sponsor:** Azadeh Vosoughi, Christopher Iannello (ECE Department)

## Project Summary

A cabin/habitat environmental monitoring system that tracks air-quality and safety-relevant conditions (CO, CO2, O2, VOC, temperature, humidity) and alerts occupants to hazardous conditions. Target requirements are derived from NASA-STD-3001 Vol 2 Rev E (crew health environmental limits).

Two design variants are under consideration:
- **With environmental resistance** — hardened enclosure (e.g. aluminum) rated for temperature/moisture extremes, for use in harsher test environments.
- **Without environmental resistance** — standard indoor cabin monitoring with a basic protective enclosure; data reported over Bluetooth/WiFi to a phone app or web dashboard.

### Core Hardware
- **Sensing:** Bosch BME688 (VOC + gas + temperature/humidity/pressure, I²C)
- **MCU:** ESP32 (sensor fusion, RTOS, WiFi/BT)
- **Display:** ILI9341 or similar (SPI)
- **Storage:** microSD (data logging), DS3231 RTC (timestamps)
- **Power:** LM-family regulator(s), power distribution board
- **Alerts:** Piezo buzzer, status LED(s)

### Core Software
RTOS task breakdown: timer/interrupt scheduling, per-sensor capture & rolling-average analysis (temperature, humidity, CO2, CO), threshold-based alarm response, WiFi/BT status reporting, and SD data logging.

### Candidate Test Environments
Small Bodies Lab (FSI), ASTRA Lab, Vasu Lab, Propulsion and Energy Research Lab (UCF).

## Repo Contents

This repo holds the technical project — firmware, hardware design files, and test code. The SD1/SD2 deadline tracker (Trello board) and its Discord digest automation live in a separate repo.

## Course Deliverables (SD1, Fall 2026)

Divide & Conquer Report → Midterm Milestone Report → Draft Final Report + Mini Demo Video → Final Report. See the team's Trello board for the full dated task breakdown.
