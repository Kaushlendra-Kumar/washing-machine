#ifndef WASHING_MACHINE_HPP
#define WASHING_MACHINE_HPP

#include "StateMachine.hpp"
#include "EventEngine.hpp"
#include "DoorSystem.hpp"
#include "WaterSystem.hpp"
#include "MotorSystem.hpp"
#include "ConfigManager.hpp"
#include "WashMode.hpp"
#include "Types.hpp"

#include <atomic>
#include <thread>
#include <string>

class WashingMachine {
private:
    StateMachine stateMachine;
    EventEngine eventEngine;
    DoorSystem door;
    WaterSystem water;
    MotorSystem motor;
    ConfigManager config;

    int currentModeIndex;
    float loadWeight;
    float cycleProgress;
    float cycleTimeElapsed;
    float totalCycleTime;
    float phaseTimeElapsed;
    float currentPhaseTime;
    FaultCode currentFault;

    std::atomic<bool> running;
    std::atomic<bool> simulationRunning;
    std::thread simulationThread;

    void setupCallbacks();
    void handleEvent(const Event& event);
    void simulationLoop();
    void updateSimulation(float deltaTime);

    void onStateEnter(State newState, State oldState);
    void onStateExit(State oldState, State newState);

    void startFillPhase();
    void startWashPhase();
    void startRinsePhase();
    void startSpinPhase();
    void startDrainPhase();

    void executeEmergencyStop();
    bool validateStart() const;

    float calculateFillTime() const;
    float calculateWashTime() const;
    float calculateRinseTime() const;
    float calculateSpinTime() const;
    float calculateDrainTime() const;
    float calculateTotalCycleTime() const;

public:
    WashingMachine();
    ~WashingMachine();

    bool initialize(const std::string& configPath = "");
    void run();
    void shutdown();

    void openDoor();
    void closeDoor();
    void selectMode(int modeIndex);
    void setLoad(float kg);
    void start();
    void pause();
    void resume();
    void stop();
    void emergencyStop();
    void clearFault();

    SystemStatus getStatus() const;
    const WashMode& getCurrentMode() const;
    State getCurrentState() const;
    const ConfigManager& getConfigManager() const;

    void processEvents();
    bool isRunning() const;
};

#endif