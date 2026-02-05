#ifndef MOTOR_SYSTEM_HPP
#define MOTOR_SYSTEM_HPP

#include "Types.hpp"
#include <functional>

class MotorSystem {
private:
    int currentRPM;
    int targetRPM;
    bool running;
    Direction direction;
    int rampRate;
    std::function<void(EventType)> eventCallback;

public:
    MotorSystem();

    void setEventCallback(std::function<void(EventType)> callback);

    void start(int rpm, Direction dir = Direction::Clockwise);
    void stop();
    void setSpeed(int rpm);
    void setDirection(Direction dir);

    void update(float deltaTimeSeconds);

    int getCurrentRPM() const;
    int getTargetRPM() const;
    bool isRunning() const;
    Direction getDirection() const;

    void emergencyStop();
    void reset();
};

#endif