#include "StateMachine.hpp"

StateMachine::StateMachine()
    : currentState(State::Idle),
      previousState(State::Idle),
      pausedFromState(State::Idle) {
    initializeTransitions();
}

void StateMachine::initializeTransitions() {
    transitionTable[State::Idle][EventType::CMD_OPEN_DOOR] = State::DoorOpen;
    transitionTable[State::Idle][EventType::CMD_SELECT_MODE] = State::Ready;

    transitionTable[State::DoorOpen][EventType::CMD_CLOSE_DOOR] = State::Idle;

    transitionTable[State::Ready][EventType::CMD_OPEN_DOOR] = State::DoorOpen;
    transitionTable[State::Ready][EventType::CMD_START] = State::Filling;
    transitionTable[State::Ready][EventType::CMD_STOP] = State::Idle;
    transitionTable[State::Ready][EventType::CMD_SELECT_MODE] = State::Ready;

    transitionTable[State::Filling][EventType::SYS_WATER_LEVEL_REACHED] = State::Washing;
    transitionTable[State::Filling][EventType::CMD_PAUSE] = State::Paused;
    transitionTable[State::Filling][EventType::CMD_EMERGENCY] = State::EmergencyStop;
    transitionTable[State::Filling][EventType::FAULT_WATER_UNAVAILABLE] = State::Fault;
    transitionTable[State::Filling][EventType::CMD_STOP] = State::Draining;

    transitionTable[State::Washing][EventType::SYS_WASH_COMPLETE] = State::Rinsing;
    transitionTable[State::Washing][EventType::CMD_PAUSE] = State::Paused;
    transitionTable[State::Washing][EventType::CMD_EMERGENCY] = State::EmergencyStop;
    transitionTable[State::Washing][EventType::CMD_STOP] = State::Draining;

    transitionTable[State::Rinsing][EventType::SYS_RINSE_COMPLETE] = State::Spinning;
    transitionTable[State::Rinsing][EventType::CMD_PAUSE] = State::Paused;
    transitionTable[State::Rinsing][EventType::CMD_EMERGENCY] = State::EmergencyStop;
    transitionTable[State::Rinsing][EventType::CMD_STOP] = State::Draining;

    transitionTable[State::Spinning][EventType::SYS_SPIN_COMPLETE] = State::Draining;
    transitionTable[State::Spinning][EventType::CMD_PAUSE] = State::Paused;
    transitionTable[State::Spinning][EventType::CMD_EMERGENCY] = State::EmergencyStop;
    transitionTable[State::Spinning][EventType::CMD_STOP] = State::Draining;

    transitionTable[State::Draining][EventType::SYS_DRAIN_COMPLETE] = State::Completed;
    transitionTable[State::Draining][EventType::CMD_EMERGENCY] = State::EmergencyStop;

    transitionTable[State::Completed][EventType::CMD_OPEN_DOOR] = State::DoorOpen;
    transitionTable[State::Completed][EventType::CMD_STOP] = State::Idle;
    transitionTable[State::Completed][EventType::CMD_SELECT_MODE] = State::Ready;

    transitionTable[State::Paused][EventType::CMD_RESUME] = State::Filling;
    transitionTable[State::Paused][EventType::CMD_STOP] = State::Draining;
    transitionTable[State::Paused][EventType::CMD_EMERGENCY] = State::EmergencyStop;

    transitionTable[State::EmergencyStop][EventType::CMD_STOP] = State::Idle;
    transitionTable[State::EmergencyStop][EventType::SYS_DRAIN_COMPLETE] = State::Idle;

    transitionTable[State::Fault][EventType::FAULT_CLEARED] = State::Idle;
    transitionTable[State::Fault][EventType::CMD_STOP] = State::Idle;
}

State StateMachine::getCurrentState() const {
    return currentState;
}

State StateMachine::getPreviousState() const {
    return previousState;
}

State StateMachine::getPausedFromState() const {
    return pausedFromState;
}

void StateMachine::setPausedFromState(State state) {
    pausedFromState = state;
}

bool StateMachine::canTransition(EventType event) const {
    auto stateIt = transitionTable.find(currentState);
    if (stateIt == transitionTable.end()) {
        return false;
    }
    return stateIt->second.find(event) != stateIt->second.end();
}

bool StateMachine::transition(EventType event) {
    if (!canTransition(event)) {
        return false;
    }

    State oldState = currentState;
    State newState = transitionTable.at(currentState).at(event);

    auto exitIt = onExitCallbacks.find(oldState);
    if (exitIt != onExitCallbacks.end()) {
        for (const auto& callback : exitIt->second) {
            callback(oldState, newState);
        }
    }

    previousState = currentState;
    currentState = newState;

    auto enterIt = onEnterCallbacks.find(newState);
    if (enterIt != onEnterCallbacks.end()) {
        for (const auto& callback : enterIt->second) {
            callback(newState, oldState);
        }
    }

    return true;
}

void StateMachine::forceState(State state) {
    State oldState = currentState;

    auto exitIt = onExitCallbacks.find(oldState);
    if (exitIt != onExitCallbacks.end()) {
        for (const auto& callback : exitIt->second) {
            callback(oldState, state);
        }
    }

    previousState = currentState;
    currentState = state;

    auto enterIt = onEnterCallbacks.find(state);
    if (enterIt != onEnterCallbacks.end()) {
        for (const auto& callback : enterIt->second) {
            callback(state, oldState);
        }
    }
}

void StateMachine::registerOnEnter(State state, StateCallback callback) {
    onEnterCallbacks[state].push_back(callback);
}

void StateMachine::registerOnExit(State state, StateCallback callback) {
    onExitCallbacks[state].push_back(callback);
}

bool StateMachine::isActiveState() const {
    return currentState == State::Filling ||
           currentState == State::Washing ||
           currentState == State::Rinsing ||
           currentState == State::Spinning ||
           currentState == State::Draining;
}

bool StateMachine::isSafeToOpenDoor() const {
    return currentState == State::Idle ||
           currentState == State::DoorOpen ||
           currentState == State::Ready ||
           currentState == State::Completed;
}

void StateMachine::reset() {
    currentState = State::Idle;
    previousState = State::Idle;
    pausedFromState = State::Idle;
}