#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include "Types.hpp"
#include <map>
#include <functional>
#include <vector>

class StateMachine {
public:
    using StateCallback = std::function<void(State, State)>;

private:
    State currentState;
    State previousState;
    State pausedFromState;
    std::map<State, std::map<EventType, State>> transitionTable;
    std::map<State, std::vector<StateCallback>> onEnterCallbacks;
    std::map<State, std::vector<StateCallback>> onExitCallbacks;

    void initializeTransitions();

public:
    StateMachine();

    State getCurrentState() const;
    State getPreviousState() const;
    State getPausedFromState() const;
    void setPausedFromState(State state);

    bool canTransition(EventType event) const;
    bool transition(EventType event);
    void forceState(State state);

    void registerOnEnter(State state, StateCallback callback);
    void registerOnExit(State state, StateCallback callback);

    bool isActiveState() const;
    bool isSafeToOpenDoor() const;

    void reset();
};

#endif