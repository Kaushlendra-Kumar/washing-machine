#ifndef DOOR_SYSTEM_HPP
#define DOOR_SYSTEM_HPP

#include "Types.hpp"
#include <functional>

class DoorSystem {
private:
    bool open;
    bool locked;
    std::function<void(EventType)> eventCallback;

public:
    DoorSystem();

    void setEventCallback(std::function<void(EventType)> callback);

    bool openDoor();
    bool closeDoor();
    void lock();
    void unlock();

    bool isOpen() const;
    bool isLocked() const;
    bool canOpen() const;
    DoorStatus getStatus() const;

    std::string getStatusString() const;
    void reset();
};

#endif