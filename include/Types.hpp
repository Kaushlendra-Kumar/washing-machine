#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <chrono>

enum class State {
    Idle,
    DoorOpen,
    Ready,
    Filling,
    Washing,
    Rinsing,
    Spinning,
    Draining,
    Completed,
    Paused,
    EmergencyStop,
    Fault
};

enum class EventType {
    CMD_OPEN_DOOR,
    CMD_CLOSE_DOOR,
    CMD_SELECT_MODE,
    CMD_START,
    CMD_PAUSE,
    CMD_RESUME,
    CMD_STOP,
    CMD_EMERGENCY,
    CMD_SET_LOAD,
    
    SYS_WATER_LEVEL_REACHED,
    SYS_WASH_COMPLETE,
    SYS_RINSE_COMPLETE,
    SYS_SPIN_COMPLETE,
    SYS_DRAIN_COMPLETE,
    SYS_CYCLE_COMPLETE,
    
    TIMER_TICK,
    TIMER_TIMEOUT,
    
    FAULT_WATER_UNAVAILABLE,
    FAULT_OVERLOAD,
    FAULT_DOOR,
    FAULT_MOTOR,
    FAULT_CLEARED
};

enum class DoorStatus {
    Open,
    ClosedUnlocked,
    ClosedLocked
};

enum class Direction {
    Clockwise,
    CounterClockwise,
    Stopped
};

enum class FaultCode {
    None,
    WaterUnavailable,
    Overload,
    DoorFault,
    MotorFault,
    Timeout
};

struct SystemStatus {
    State state;
    DoorStatus doorStatus;
    float waterLevel;
    float targetWaterLevel;
    int motorRPM;
    float loadKg;
    int modeIndex;
    std::string modeName;
    float progressPercent;
    int remainingSeconds;
    FaultCode fault;
};

inline std::string stateToString(State state) {
    switch (state) {
        case State::Idle: return "Idle";
        case State::DoorOpen: return "Door Open";
        case State::Ready: return "Ready";
        case State::Filling: return "Filling";
        case State::Washing: return "Washing";
        case State::Rinsing: return "Rinsing";
        case State::Spinning: return "Spinning";
        case State::Draining: return "Draining";
        case State::Completed: return "Completed";
        case State::Paused: return "Paused";
        case State::EmergencyStop: return "Emergency Stop";
        case State::Fault: return "Fault";
        default: return "Unknown";
    }
}

inline std::string eventTypeToString(EventType type) {
    switch (type) {
        case EventType::CMD_OPEN_DOOR: return "CMD_OPEN_DOOR";
        case EventType::CMD_CLOSE_DOOR: return "CMD_CLOSE_DOOR";
        case EventType::CMD_SELECT_MODE: return "CMD_SELECT_MODE";
        case EventType::CMD_START: return "CMD_START";
        case EventType::CMD_PAUSE: return "CMD_PAUSE";
        case EventType::CMD_RESUME: return "CMD_RESUME";
        case EventType::CMD_STOP: return "CMD_STOP";
        case EventType::CMD_EMERGENCY: return "CMD_EMERGENCY";
        case EventType::CMD_SET_LOAD: return "CMD_SET_LOAD";
        case EventType::SYS_WATER_LEVEL_REACHED: return "SYS_WATER_LEVEL_REACHED";
        case EventType::SYS_WASH_COMPLETE: return "SYS_WASH_COMPLETE";
        case EventType::SYS_RINSE_COMPLETE: return "SYS_RINSE_COMPLETE";
        case EventType::SYS_SPIN_COMPLETE: return "SYS_SPIN_COMPLETE";
        case EventType::SYS_DRAIN_COMPLETE: return "SYS_DRAIN_COMPLETE";
        case EventType::SYS_CYCLE_COMPLETE: return "SYS_CYCLE_COMPLETE";
        case EventType::TIMER_TICK: return "TIMER_TICK";
        case EventType::TIMER_TIMEOUT: return "TIMER_TIMEOUT";
        case EventType::FAULT_WATER_UNAVAILABLE: return "FAULT_WATER_UNAVAILABLE";
        case EventType::FAULT_OVERLOAD: return "FAULT_OVERLOAD";
        case EventType::FAULT_DOOR: return "FAULT_DOOR";
        case EventType::FAULT_MOTOR: return "FAULT_MOTOR";
        case EventType::FAULT_CLEARED: return "FAULT_CLEARED";
        default: return "Unknown";
    }
}

inline std::string doorStatusToString(DoorStatus status) {
    switch (status) {
        case DoorStatus::Open: return "Open";
        case DoorStatus::ClosedUnlocked: return "Closed (Unlocked)";
        case DoorStatus::ClosedLocked: return "Closed (Locked)";
        default: return "Unknown";
    }
}

inline std::string faultCodeToString(FaultCode fault) {
    switch (fault) {
        case FaultCode::None: return "None";
        case FaultCode::WaterUnavailable: return "Water Unavailable";
        case FaultCode::Overload: return "Overload";
        case FaultCode::DoorFault: return "Door Fault";
        case FaultCode::MotorFault: return "Motor Fault";
        case FaultCode::Timeout: return "Timeout";
        default: return "Unknown";
    }
}

#endif