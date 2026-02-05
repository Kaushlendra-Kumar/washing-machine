#include "WashingMachine.hpp"
#include <iostream>
#include <iomanip>

WashingMachine::WashingMachine()
    : currentModeIndex(0),
      loadWeight(0.0f),
      cycleProgress(0.0f),
      cycleTimeElapsed(0.0f),
      totalCycleTime(0.0f),
      phaseTimeElapsed(0.0f),
      currentPhaseTime(0.0f),
      currentFault(FaultCode::None),
      running(false),
      simulationRunning(false) {}

WashingMachine::~WashingMachine() {
    shutdown();
}

bool WashingMachine::initialize(const std::string& configPath) {
    if (!configPath.empty()) {
        config.loadConfig(configPath);
    }

    setupCallbacks();

    water.setEventCallback([this](EventType type) {
        eventEngine.pushEvent(type);
    });

    motor.setEventCallback([this](EventType type) {
        eventEngine.pushEvent(type);
    });

    eventEngine.start();
    running = true;

    return true;
}

void WashingMachine::setupCallbacks() {
    stateMachine.registerOnEnter(State::Filling, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });

    stateMachine.registerOnEnter(State::Washing, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });

    stateMachine.registerOnEnter(State::Rinsing, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });

    stateMachine.registerOnEnter(State::Spinning, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });

    stateMachine.registerOnEnter(State::Draining, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });

    stateMachine.registerOnEnter(State::Completed, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });

    stateMachine.registerOnEnter(State::EmergencyStop, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });

    stateMachine.registerOnEnter(State::Paused, [this](State newState, State oldState) {
        onStateEnter(newState, oldState);
    });
}

void WashingMachine::onStateEnter(State newState, State oldState) {
    switch (newState) {
        case State::Filling:
            startFillPhase();
            break;
        case State::Washing:
            startWashPhase();
            break;
        case State::Rinsing:
            startRinsePhase();
            break;
        case State::Spinning:
            startSpinPhase();
            break;
        case State::Draining:
            startDrainPhase();
            break;
        case State::Completed:
            motor.stop();
            door.unlock();
            std::cout << "\n*** CYCLE COMPLETE ***\n" << std::endl;
            break;
        case State::EmergencyStop:
            executeEmergencyStop();
            break;
        case State::Paused:
            stateMachine.setPausedFromState(oldState);
            motor.stop();
            water.stopFilling();
            break;
        default:
            break;
    }
}

void WashingMachine::onStateExit(State oldState, State newState) {
}

void WashingMachine::startFillPhase() {
    const WashMode& mode = getCurrentMode();
    float targetWater = mode.getAdjustedWaterLevel(loadWeight);
    water.startFilling(targetWater);
    currentPhaseTime = calculateFillTime();
    phaseTimeElapsed = 0.0f;
    door.lock();
}

void WashingMachine::startWashPhase() {
    const WashMode& mode = getCurrentMode();
    motor.start(mode.spinSpeedRPM / 2, Direction::Clockwise);
    currentPhaseTime = calculateWashTime();
    phaseTimeElapsed = 0.0f;
}

void WashingMachine::startRinsePhase() {
    motor.start(400, Direction::CounterClockwise);
    currentPhaseTime = calculateRinseTime();
    phaseTimeElapsed = 0.0f;
}

void WashingMachine::startSpinPhase() {
    const WashMode& mode = getCurrentMode();
    motor.start(mode.spinSpeedRPM, Direction::Clockwise);
    currentPhaseTime = calculateSpinTime();
    phaseTimeElapsed = 0.0f;
}

void WashingMachine::startDrainPhase() {
    motor.stop();
    water.startDraining();
    currentPhaseTime = calculateDrainTime();
    phaseTimeElapsed = 0.0f;
}

void WashingMachine::executeEmergencyStop() {
    motor.emergencyStop();
    water.stopFilling();
    water.startDraining();
    std::cout << "\n!!! EMERGENCY STOP ACTIVATED !!!\n" << std::endl;
}

bool WashingMachine::validateStart() const {
    if (door.isOpen()) {
        std::cout << "Error: Door is open. Please close the door.\n";
        return false;
    }

    if (loadWeight <= 0) {
        std::cout << "Error: No load set. Use 'load <kg>' command.\n";
        return false;
    }

    if (loadWeight > 6.0f) {
        std::cout << "Error: Load exceeds maximum capacity (6 kg).\n";
        return false;
    }

    if (!water.checkReservoir()) {
        std::cout << "Error: Water reservoir is low.\n";
        return false;
    }

    return true;
}

float WashingMachine::calculateFillTime() const {
    const WashMode& mode = getCurrentMode();
    float waterNeeded = mode.getAdjustedWaterLevel(loadWeight);
    return waterNeeded / 10.0f;
}

float WashingMachine::calculateWashTime() const {
    const WashMode& mode = getCurrentMode();
    return mode.getAdjustedDuration(loadWeight) * 0.5f * 60.0f;
}

float WashingMachine::calculateRinseTime() const {
    const WashMode& mode = getCurrentMode();
    return mode.getAdjustedDuration(loadWeight) * 0.25f * 60.0f;
}

float WashingMachine::calculateSpinTime() const {
    const WashMode& mode = getCurrentMode();
    return mode.getAdjustedDuration(loadWeight) * 0.15f * 60.0f;
}

float WashingMachine::calculateDrainTime() const {
    return water.getCurrentLevel() / 15.0f;
}

float WashingMachine::calculateTotalCycleTime() const {
    return calculateFillTime() + calculateWashTime() + calculateRinseTime() +
           calculateSpinTime() + calculateDrainTime();
}

void WashingMachine::run() {
    simulationRunning = true;
    simulationThread = std::thread(&WashingMachine::simulationLoop, this);
}

void WashingMachine::simulationLoop() {
    auto lastTime = std::chrono::steady_clock::now();

    while (simulationRunning) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        processEvents();
        updateSimulation(deltaTime);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void WashingMachine::updateSimulation(float deltaTime) {
    State state = stateMachine.getCurrentState();

    if (stateMachine.isActiveState()) {
        water.update(deltaTime);
        motor.update(deltaTime);

        cycleTimeElapsed += deltaTime;
        phaseTimeElapsed += deltaTime;

        if (totalCycleTime > 0) {
            cycleProgress = (cycleTimeElapsed / totalCycleTime) * 100.0f;
            if (cycleProgress > 100.0f) cycleProgress = 100.0f;
        }

        switch (state) {
            case State::Washing:
                if (phaseTimeElapsed >= currentPhaseTime) {
                    eventEngine.pushEvent(EventType::SYS_WASH_COMPLETE);
                }
                break;
            case State::Rinsing:
                if (phaseTimeElapsed >= currentPhaseTime) {
                    eventEngine.pushEvent(EventType::SYS_RINSE_COMPLETE);
                }
                break;
            case State::Spinning:
                if (phaseTimeElapsed >= currentPhaseTime) {
                    eventEngine.pushEvent(EventType::SYS_SPIN_COMPLETE);
                }
                break;
            default:
                break;
        }
    }

    if (state == State::Paused) {
        motor.update(deltaTime);
    }

    if (state == State::EmergencyStop) {
        water.update(deltaTime);
        if (water.getCurrentLevel() <= 0 && motor.getCurrentRPM() == 0) {
            door.unlock();
        }
    }
}

void WashingMachine::processEvents() {
    while (eventEngine.hasEvents()) {
        auto eventOpt = eventEngine.popEvent();
        if (eventOpt) {
            handleEvent(*eventOpt);
        }
    }
}

void WashingMachine::handleEvent(const Event& event) {
    EventType type = event.getType();

    if (type == EventType::CMD_SELECT_MODE && event.hasData()) {
        currentModeIndex = event.getData<int>();
    }

    if (type == EventType::CMD_SET_LOAD && event.hasData()) {
        loadWeight = event.getData<float>();
    }

    stateMachine.transition(type);
}

void WashingMachine::shutdown() {
    simulationRunning = false;
    running = false;
    eventEngine.stop();

    if (simulationThread.joinable()) {
        simulationThread.join();
    }
}

void WashingMachine::openDoor() {
    if (door.canOpen()) {
        door.openDoor();
        stateMachine.transition(EventType::CMD_OPEN_DOOR);
    } else {
        std::cout << "Cannot open door: Machine is locked during operation.\n";
    }
}

void WashingMachine::closeDoor() {
    door.closeDoor();
    stateMachine.transition(EventType::CMD_CLOSE_DOOR);
}

void WashingMachine::selectMode(int modeIndex) {
    if (modeIndex < 0 || modeIndex >= config.getModeCount()) {
        std::cout << "Invalid mode. Please select 1-" << config.getModeCount() << ".\n";
        return;
    }

    if (stateMachine.isActiveState()) {
        std::cout << "Cannot change mode during active cycle.\n";
        return;
    }

    currentModeIndex = modeIndex;
    eventEngine.pushEvent(EventType::CMD_SELECT_MODE, modeIndex);

    const WashMode& mode = config.getMode(modeIndex);
    std::cout << "Mode selected: " << mode.name << "\n";
}

void WashingMachine::setLoad(float kg) {
    if (stateMachine.isActiveState()) {
        std::cout << "Cannot change load during active cycle.\n";
        return;
    }

    if (kg < 0) {
        std::cout << "Load cannot be negative.\n";
        return;
    }

    if (kg > 6.0f) {
        std::cout << "Warning: Maximum capacity is 6 kg.\n";
    }

    loadWeight = kg;
    eventEngine.pushEvent(EventType::CMD_SET_LOAD, kg);
    std::cout << "Load set to " << kg << " kg.\n";
}

void WashingMachine::start() {
    if (!validateStart()) {
        return;
    }

    totalCycleTime = calculateTotalCycleTime();
    cycleTimeElapsed = 0.0f;
    cycleProgress = 0.0f;

    eventEngine.pushEvent(EventType::CMD_START);
    std::cout << "Starting wash cycle...\n";
}

void WashingMachine::pause() {
    if (stateMachine.isActiveState()) {
        eventEngine.pushEvent(EventType::CMD_PAUSE);
        std::cout << "Cycle paused.\n";
    } else {
        std::cout << "No active cycle to pause.\n";
    }
}

void WashingMachine::resume() {
    if (stateMachine.getCurrentState() == State::Paused) {
        State resumeState = stateMachine.getPausedFromState();
        stateMachine.forceState(resumeState);

        switch (resumeState) {
            case State::Filling:
                startFillPhase();
                break;
            case State::Washing:
                startWashPhase();
                break;
            case State::Rinsing:
                startRinsePhase();
                break;
            case State::Spinning:
                startSpinPhase();
                break;
            default:
                break;
        }

        std::cout << "Cycle resumed.\n";
    } else {
        std::cout << "No paused cycle to resume.\n";
    }
}

void WashingMachine::stop() {
    State state = stateMachine.getCurrentState();

    if (state == State::Idle || state == State::DoorOpen) {
        std::cout << "Machine is already stopped.\n";
        return;
    }

    if (stateMachine.isActiveState() || state == State::Paused) {
        motor.stop();
        water.stopFilling();
        if (water.getCurrentLevel() > 0) {
            water.startDraining();
            stateMachine.forceState(State::Draining);
            std::cout << "Stopping... Draining water.\n";
        } else {
            door.unlock();
            stateMachine.forceState(State::Idle);
            std::cout << "Machine stopped.\n";
        }
    } else {
        eventEngine.pushEvent(EventType::CMD_STOP);
    }
}

void WashingMachine::emergencyStop() {
    eventEngine.pushEvent(EventType::CMD_EMERGENCY);
}

void WashingMachine::clearFault() {
    if (stateMachine.getCurrentState() == State::Fault) {
        currentFault = FaultCode::None;
        eventEngine.pushEvent(EventType::FAULT_CLEARED);
        std::cout << "Fault cleared.\n";
    }
}

SystemStatus WashingMachine::getStatus() const {
    SystemStatus status;
    status.state = stateMachine.getCurrentState();
    status.doorStatus = door.getStatus();
    status.waterLevel = water.getCurrentLevel();
    status.targetWaterLevel = water.getTargetLevel();
    status.motorRPM = motor.getCurrentRPM();
    status.loadKg = loadWeight;
    status.modeIndex = currentModeIndex;
    status.modeName = config.getMode(currentModeIndex).name;
    status.progressPercent = cycleProgress;

    float remaining = totalCycleTime - cycleTimeElapsed;
    status.remainingSeconds = (remaining > 0) ? static_cast<int>(remaining) : 0;

    status.fault = currentFault;
    return status;
}

const WashMode& WashingMachine::getCurrentMode() const {
    return config.getMode(currentModeIndex);
}

State WashingMachine::getCurrentState() const {
    return stateMachine.getCurrentState();
}

const ConfigManager& WashingMachine::getConfigManager() const {
    return config;
}

bool WashingMachine::isRunning() const {
    return running;
}