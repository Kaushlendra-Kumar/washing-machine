#include "DoorSystem.hpp"

DoorSystem::DoorSystem() : open(true), locked(false), eventCallback(nullptr) {}

void DoorSystem::setEventCallback(std::function<void(EventType)> callback) {
    eventCallback = callback;
}

bool DoorSystem::openDoor() {
    if (locked) {
        return false;
    }
    if (!open) {
        open = true;
        if (eventCallback) {
            eventCallback(EventType::CMD_OPEN_DOOR);
        }
    }
    return true;
}

bool DoorSystem::closeDoor() {
    if (open) {
        open = false;
        if (eventCallback) {
            eventCallback(EventType::CMD_CLOSE_DOOR);
        }
    }
    return true;
}

void DoorSystem::lock() {
    if (!open) {
        locked = true;
    }
}

void DoorSystem::unlock() {
    locked = false;
}

bool DoorSystem::isOpen() const {
    return open;
}

bool DoorSystem::isLocked() const {
    return locked;
}

bool DoorSystem::canOpen() const {
    return !locked;
}

DoorStatus DoorSystem::getStatus() const {
    if (open) {
        return DoorStatus::Open;
    }
    return locked ? DoorStatus::ClosedLocked : DoorStatus::ClosedUnlocked;
}

std::string DoorSystem::getStatusString() const {
    return doorStatusToString(getStatus());
}

void DoorSystem::reset() {
    open = true;
    locked = false;
}