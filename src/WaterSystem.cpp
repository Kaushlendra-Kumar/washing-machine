#include "WaterSystem.hpp"

WaterSystem::WaterSystem()
    : currentLevel(0.0f),
      targetLevel(0.0f),
      reservoirLevel(100.0f),
      maxReservoir(100.0f),
      fillRate(10.0f),
      drainRate(15.0f),
      inletValveOpen(false),
      drainValveOpen(false),
      lowThreshold(10.0f),
      eventCallback(nullptr) {}

void WaterSystem::setEventCallback(std::function<void(EventType)> callback) {
    eventCallback = callback;
}

void WaterSystem::startFilling(float targetLiters) {
    if (reservoirLevel < lowThreshold) {
        if (!autoReplenish()) {
            if (eventCallback) {
                eventCallback(EventType::FAULT_WATER_UNAVAILABLE);
            }
            return;
        }
    }
    targetLevel = targetLiters;
    inletValveOpen = true;
    drainValveOpen = false;
}

void WaterSystem::stopFilling() {
    inletValveOpen = false;
}

void WaterSystem::startDraining() {
    drainValveOpen = true;
    inletValveOpen = false;
}

void WaterSystem::stopDraining() {
    drainValveOpen = false;
}

void WaterSystem::update(float deltaTimeSeconds) {
    if (inletValveOpen && currentLevel < targetLevel) {
        float fillAmount = fillRate * deltaTimeSeconds;
        float available = reservoirLevel;

        if (fillAmount > available) {
            fillAmount = available;
        }

        currentLevel += fillAmount;
        reservoirLevel -= fillAmount;

        if (currentLevel >= targetLevel) {
            currentLevel = targetLevel;
            inletValveOpen = false;
            if (eventCallback) {
                eventCallback(EventType::SYS_WATER_LEVEL_REACHED);
            }
        }

        if (reservoirLevel < lowThreshold) {
            autoReplenish();
        }
    }

    if (drainValveOpen && currentLevel > 0) {
        float drainAmount = drainRate * deltaTimeSeconds;
        currentLevel -= drainAmount;

        if (currentLevel <= 0) {
            currentLevel = 0;
            drainValveOpen = false;
            if (eventCallback) {
                eventCallback(EventType::SYS_DRAIN_COMPLETE);
            }
        }
    }
}

bool WaterSystem::checkReservoir() const {
    return reservoirLevel >= lowThreshold;
}

bool WaterSystem::autoReplenish() {
    reservoirLevel = maxReservoir;
    return true;
}

float WaterSystem::getCurrentLevel() const {
    return currentLevel;
}

float WaterSystem::getTargetLevel() const {
    return targetLevel;
}

float WaterSystem::getReservoirLevel() const {
    return reservoirLevel;
}

float WaterSystem::getMaxReservoir() const {
    return maxReservoir;
}

bool WaterSystem::isFilling() const {
    return inletValveOpen;
}

bool WaterSystem::isDraining() const {
    return drainValveOpen;
}

void WaterSystem::setReservoirLevel(float level) {
    reservoirLevel = level;
    if (reservoirLevel > maxReservoir) {
        reservoirLevel = maxReservoir;
    }
    if (reservoirLevel < 0) {
        reservoirLevel = 0;
    }
}

void WaterSystem::reset() {
    currentLevel = 0.0f;
    targetLevel = 0.0f;
    inletValveOpen = false;
    drainValveOpen = false;
    reservoirLevel = maxReservoir;
}