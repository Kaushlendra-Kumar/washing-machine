#include "MotorSystem.hpp"
#include <cmath>

MotorSystem::MotorSystem()
    : currentRPM(0),
      targetRPM(0),
      running(false),
      direction(Direction::Stopped),
      rampRate(200),
      eventCallback(nullptr) {}

void MotorSystem::setEventCallback(std::function<void(EventType)> callback) {
    eventCallback = callback;
}

void MotorSystem::start(int rpm, Direction dir) {
    targetRPM = rpm;
    direction = dir;
    running = true;
}

void MotorSystem::stop() {
    targetRPM = 0;
    running = false;
}

void MotorSystem::setSpeed(int rpm) {
    targetRPM = rpm;
    if (rpm > 0 && !running) {
        running = true;
    }
}

void MotorSystem::setDirection(Direction dir) {
    direction = dir;
}

void MotorSystem::update(float deltaTimeSeconds) {
    if (!running && currentRPM == 0) {
        direction = Direction::Stopped;
        return;
    }

    int rampAmount = static_cast<int>(rampRate * deltaTimeSeconds);

    if (currentRPM < targetRPM) {
        currentRPM += rampAmount;
        if (currentRPM > targetRPM) {
            currentRPM = targetRPM;
        }
    } else if (currentRPM > targetRPM) {
        currentRPM -= rampAmount;
        if (currentRPM < targetRPM) {
            currentRPM = targetRPM;
        }
    }

    if (!running && currentRPM == 0) {
        direction = Direction::Stopped;
    }
}

int MotorSystem::getCurrentRPM() const {
    return currentRPM;
}

int MotorSystem::getTargetRPM() const {
    return targetRPM;
}

bool MotorSystem::isRunning() const {
    return running || currentRPM > 0;
}

Direction MotorSystem::getDirection() const {
    return direction;
}

void MotorSystem::emergencyStop() {
    running = false;
    targetRPM = 0;
    currentRPM = 0;
    direction = Direction::Stopped;
}

void MotorSystem::reset() {
    currentRPM = 0;
    targetRPM = 0;
    running = false;
    direction = Direction::Stopped;
}