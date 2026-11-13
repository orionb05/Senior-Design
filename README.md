# Space Cabin Environmental Safety Monitor (SCESM)

SCESM is a distributed environmental-safety monitoring system for a spacecraft-cabin prototype. This repository is dedicated to the software implementation: embedded firmware for ESP32-S3 collector nodes and a central hub, plus the companion mobile application.

The system collects environmental data from multiple cabin locations, evaluates hazards locally and centrally, coordinates alerts over Bluetooth Low Energy (BLE), preserves system history, and provides crew-facing status and configuration.

> SCESM is a senior-design proof of concept. It uses NASA guidance as a design reference and does not claim NASA compliance, flight qualification, or space certification.

## System overview

```text
 Environmental sensors             BLE network                 Crew interface
 ┌────────────────────┐       ┌──────────────────┐       ┌──────────────────┐
 │ Collector node(s)  │ ────> │ Central hub      │ <──── │ Mobile app       │
 │ validate + fuse    │       │ aggregate + log  │       │ monitor + config │
 │ local alerts       │ <──── │ coordinate alerts│       └──────────────────┘
 └────────────────────┘       └────────┬─────────┘
                                       │
                                       v
                                  microSD history
```

Normal operation uses a hub-and-spoke BLE topology. Collector nodes can provide local alerts and are designed to support direct-to-mobile fallback during a hub outage. Selective peer-to-peer alarm propagation and predictive cross-node analysis are stretch capabilities.

## Software components

### Collector firmware

- ESP-IDF with FreeRTOS on the ESP32-S3
- Independent I²C and UART acquisition tasks
- Timestamped measurement queues and local assessment
- Sensor validation, environmental compensation, sensor fusion, threshold evaluation, persistence, and hysteresis
- Nominal and Elevated Monitoring Modes
- Local LED/buzzer alert handling
- BLE peripheral services for telemetry, node health, and alert/configuration commands

### Central-hub firmware

- BLE management for multiple collectors and the mobile client
- Collector health monitoring and fault reporting
- Source-identified telemetry aggregation and system-wide alert coordination
- Asynchronous microSD logging through a producer-consumer queue
- Application-facing JSON serialization and configuration handling
- Optional cross-location and predictive analysis

### Mobile application

- Kotlin Multiplatform with Jetpack Compose
- Layered presentation, ViewModel, data, and BLE communication architecture
- Dashboard, node detail, alerts, and settings views
- Current measurements, node health, faults, and active alarms
- Hub-first connection with direct collector fallback
- Threshold, alert-profile, and monitoring-mode commands

## Environmental data

Each collector reports:

- Carbon monoxide (CO)
- Carbon dioxide (CO₂)
- VOC-related air quality / IAQ
- Temperature
- Relative humidity
- Atmospheric pressure

Telemetry is source-identified by collector and mapped to a logical cabin zone. Embedded links use compact binary payloads; the hub translates validated data into a stable JSON format for the mobile application.

## Initial software targets

| Capability | Target |
| --- | --- |
| Simultaneous collectors | At least 2 per hub |
| Indoor unobstructed BLE range | At least 9 m |
| Collector reporting interval | ≤30 s nominal; ≤4 s elevated |
| Alert distribution | ≤2 s after confirmed decision |
| Fault indication | ≤5 s after invalid data or overdue update |
| Retained history | At least 4 hours per collector at maximum rate |

## Repository scope

This repository will contain software source, shared data definitions, build/configuration files, tests, and developer documentation for the collector, hub, and mobile application.

Hardware schematics, PCB layout, enclosure design, component research, fabrication, and the full project report belong to the broader SCESM project but are outside this repository’s implementation scope.

## Development principles

- Keep time-sensitive monitoring and alert paths independent from slow storage and UI operations.
- Treat invalid or stale measurements as faults rather than valid environmental readings.
- Use queues, synchronization, and explicit task ownership for shared buses and data.
- Make telemetry and GATT behavior versioned and deterministic across firmware and mobile code.
- Test nominal monitoring, elevated monitoring, alarm persistence/recovery, sensor faults, stale nodes, BLE loss/reconnection, and hub fallback behavior.

## Project reference

The system requirements and design rationale are defined in `G25_Midterm_Report_Revision.pdf`, particularly Chapters 2 and 7 of the project report.
