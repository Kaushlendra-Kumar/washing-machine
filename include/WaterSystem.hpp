#ifndef WATER_SYSTEM_HPP
#define WATER_SYSTEM_HPP

#include "Types.hpp"
#include <functional>

class WaterSystem {
private:
    float currentLevel;
    float targetLevel;
    float reservoirLevel;
    float maxReservoir;
    float fillRate;
    float drainRate;
    bool inletValveOpen;
    bool drainValveOpen;
    float lowThreshold;
    std::function<void(EventType)> eventCallback;

public:
    WaterSystem();

    void setEventCallback(std::function<void(EventType)> callback);

    void startFilling(float targetLiters);
    void stopFilling();
    void startDraining();
    void stopDraining();

    void update(float deltaTimeSeconds);

    bool checkReservoir() const;
    bool autoReplenish();

    float getCurrentLevel() const;
    float getTargetLevel() const;
    float getReservoirLevel() const;
    float getMaxReservoir() const;
    bool isFilling() const;
    bool isDraining() const;

    void setReservoirLevel(float level);
    void reset();
};

#endif