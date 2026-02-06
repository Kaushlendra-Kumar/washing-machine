# Washing Machine Simulator

A C++ simulation of a consumer washing machine with realistic state transitions, safety interlocks, and event-driven control flow.

## Overview

This project simulates a front-loading washing machine without physical hardware. It is designed to validate logic, concurrency handling, and event-driven control flow for embedded systems development.

## Features

- **Front Door System** - Door open/close sensor simulation with safety interlocks
- **Multiple Wash Modes** - Quick Wash, Normal, Heavy, Delicate
- **Emergency Stop** - Immediate stop with safe state transition
- **Capacity Management** - 0-6 kg load with automatic adjustments
- **Self Water Replenishing** - Auto-replenish with fault detection
- **12 Machine States** - Complete state machine implementation
- **Event-Driven Architecture** - Asynchronous event processing
- **Interactive CLI** - Command-line interface for testing

## States

| State         | Description                      |
| ------------- | -------------------------------- |
| Idle          | Machine ready, waiting for input |
| DoorOpen      | Door is open                     |
| Ready         | Mode selected, ready to start    |
| Filling       | Water filling in progress        |
| Washing       | Main wash cycle                  |
| Rinsing       | Rinse cycle                      |
| Spinning      | Spin dry cycle                   |
| Draining      | Water draining                   |
| Completed     | Cycle finished                   |
| Paused        | Cycle paused                     |
| EmergencyStop | Emergency activated              |
| Fault         | System fault detected            |

## Wash Modes

| Mode       | Duration | Spin RPM | Water Level | Temperature |
| ---------- | -------- | -------- | ----------- | ----------- |
| Quick Wash | 15 min   | 800      | 20 L        | 30°C        |
| Normal     | 45 min   | 1000     | 35 L        | 40°C        |
| Heavy      | 60 min   | 1200     | 45 L        | 60°C        |
| Delicate   | 30 min   | 400      | 30 L        | 30°C        |

## Requirements

- C++17 or later
- CMake 3.16+
- GCC/G++ (MinGW-w64 on Windows)
- Google Test (automatically downloaded)

## Building

### Windows (MinGW)

```powershell
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./washing_machine.exe   # Windows
./washing_machine       # Linux/macOS
```

## CLI Commands

| Command      | Description              |
| ------------ | ------------------------ |
| `open`       | Open the door            |
| `close`      | Close the door           |
| `load <kg>`  | Set load weight (0-6 kg) |
| `mode <1-4>` | Select wash mode         |
| `modes`      | Show available modes     |
| `start`      | Start wash cycle         |
| `pause`      | Pause current cycle      |
| `resume`     | Resume paused cycle      |
| `stop`       | Stop/cancel cycle        |
| `emergency`  | Emergency stop           |
| `status`     | Show current status      |
| `help`       | Show help message        |
| `clear`      | Clear screen             |
| `exit`       | Exit simulator           |

## Example Usage

```
washing-machine> close
washing-machine> load 3.5
Load set to 3.5 kg.
washing-machine> mode 2
Mode selected: Normal
washing-machine> start
Starting wash cycle...
washing-machine> status
+------------------------------------------------------------+
|                    MACHINE STATUS                          |
+------------------------------------------------------------+
|  State:         Filling                                    |
|  Door:          Closed (Locked)                            |
|  Mode:          Normal                                     |
|  Load:          3.5 kg                                     |
|  Water Level:   15.0 / 45.5 L                              |
|  Motor Speed:   0 RPM                                      |
|  Progress:      5.2%                                       |
|  Time Left:     42:30                                      |
+------------------------------------------------------------+
```

## Running Tests

```powershell
cd build
mingw32-make
ctest --output-on-failure
```

Or run directly:

```powershell
.\unit_tests.exe
```

## Test Coverage

### State Machine Tests

- Initial state verification
- All valid state transitions
- Invalid transition handling
- Full cycle completion
- Pause/Resume functionality

### Door System Tests

- Open/Close operations
- Lock/Unlock mechanisms
- Safety interlock verification

### Water System Tests

- Fill/Drain operations
- Reservoir management
- Auto-replenishment
- Event callbacks

### Emergency Stop Tests

- Emergency from all active states
- Motor immediate stop
- Water drainage sequence
- Door unlock after safe

### Safety Interlocks Tests

- Door lock during cycle
- Overload prevention
- Water reservoir check
- Fault state handling

## Project Structure

```
washing-machine/
├── CMakeLists.txt
├── README.md
├── config/
│   └── wash_modes.json
├── docs/
│   ├── HLD.md
│   ├── LLD.md
│   └── diagrams/
├── include/
│   ├── CLI.hpp
│   ├── ConfigManager.hpp
│   ├── DoorSystem.hpp
│   ├── Event.hpp
│   ├── EventEngine.hpp
│   ├── MotorSystem.hpp
│   ├── StateMachine.hpp
│   ├── Types.hpp
│   ├── WashMode.hpp
│   ├── WashingMachine.hpp
│   └── WaterSystem.hpp
├── src/
│   ├── CLI.cpp
│   ├── ConfigManager.cpp
│   ├── DoorSystem.cpp
│   ├── EventEngine.cpp
│   ├── MotorSystem.cpp
│   ├── StateMachine.cpp
│   ├── WashingMachine.cpp
│   ├── WaterSystem.cpp
│   └── main.cpp
└── tests/
    ├── CMakeLists.txt
    ├── test_door_system.cpp
    ├── test_emergency.cpp
    ├── test_safety_interlocks.cpp
    ├── test_state_machine.cpp
    └── test_water_system.cpp
```

## Safety Features

1. **Door Interlock** - Cannot start with door open; door locks during cycle
2. **Overload Protection** - Prevents start if load > 6 kg
3. **Water Check** - Verifies reservoir before starting
4. **Emergency Stop** - Immediate halt from any active state
5. **Fault Detection** - Transitions to fault state on errors

## Design Documents

- [High-Level Design (HLD)](docs/HLD.md)
- [Low-Level Design (LLD)](docs/LLD.md)

## Technology Stack

| Component    | Technology           |
| ------------ | -------------------- |
| Language     | C++17                |
| Build System | CMake 3.16+          |
| Testing      | Google Test 1.14     |
| Compiler     | GCC 15.2 (MinGW-w64) |
| IDE          | Visual Studio Code   |

## Author

Kaushlendra Kumar

## License

This project is for demonstration purposes.
