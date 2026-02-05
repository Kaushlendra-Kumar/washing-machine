#include <gtest/gtest.h>
#include "StateMachine.hpp"
#include "Types.hpp"

class StateMachineTest : public ::testing::Test {
protected:
    StateMachine sm;

    void SetUp() override {
        sm.reset();
    }
};

TEST_F(StateMachineTest, InitialStateIsIdle) {
    EXPECT_EQ(sm.getCurrentState(), State::Idle);
}

TEST_F(StateMachineTest, IdleToDocorOpenTransition) {
    EXPECT_TRUE(sm.canTransition(EventType::CMD_OPEN_DOOR));
    EXPECT_TRUE(sm.transition(EventType::CMD_OPEN_DOOR));
    EXPECT_EQ(sm.getCurrentState(), State::DoorOpen);
}

TEST_F(StateMachineTest, DoorOpenToIdleTransition) {
    sm.transition(EventType::CMD_OPEN_DOOR);
    EXPECT_EQ(sm.getCurrentState(), State::DoorOpen);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_CLOSE_DOOR));
    EXPECT_TRUE(sm.transition(EventType::CMD_CLOSE_DOOR));
    EXPECT_EQ(sm.getCurrentState(), State::Idle);
}

TEST_F(StateMachineTest, IdleToReadyTransition) {
    EXPECT_TRUE(sm.canTransition(EventType::CMD_SELECT_MODE));
    EXPECT_TRUE(sm.transition(EventType::CMD_SELECT_MODE));
    EXPECT_EQ(sm.getCurrentState(), State::Ready);
}

TEST_F(StateMachineTest, ReadyToFillingTransition) {
    sm.transition(EventType::CMD_SELECT_MODE);
    EXPECT_EQ(sm.getCurrentState(), State::Ready);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_START));
    EXPECT_TRUE(sm.transition(EventType::CMD_START));
    EXPECT_EQ(sm.getCurrentState(), State::Filling);
}

TEST_F(StateMachineTest, FillingToWashingTransition) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    EXPECT_EQ(sm.getCurrentState(), State::Filling);
    
    EXPECT_TRUE(sm.canTransition(EventType::SYS_WATER_LEVEL_REACHED));
    EXPECT_TRUE(sm.transition(EventType::SYS_WATER_LEVEL_REACHED));
    EXPECT_EQ(sm.getCurrentState(), State::Washing);
}

TEST_F(StateMachineTest, WashingToRinsingTransition) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    EXPECT_EQ(sm.getCurrentState(), State::Washing);
    
    EXPECT_TRUE(sm.canTransition(EventType::SYS_WASH_COMPLETE));
    EXPECT_TRUE(sm.transition(EventType::SYS_WASH_COMPLETE));
    EXPECT_EQ(sm.getCurrentState(), State::Rinsing);
}

TEST_F(StateMachineTest, RinsingToSpinningTransition) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    sm.transition(EventType::SYS_WASH_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Rinsing);
    
    EXPECT_TRUE(sm.canTransition(EventType::SYS_RINSE_COMPLETE));
    EXPECT_TRUE(sm.transition(EventType::SYS_RINSE_COMPLETE));
    EXPECT_EQ(sm.getCurrentState(), State::Spinning);
}

TEST_F(StateMachineTest, SpinningToDrainingTransition) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    sm.transition(EventType::SYS_WASH_COMPLETE);
    sm.transition(EventType::SYS_RINSE_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Spinning);
    
    EXPECT_TRUE(sm.canTransition(EventType::SYS_SPIN_COMPLETE));
    EXPECT_TRUE(sm.transition(EventType::SYS_SPIN_COMPLETE));
    EXPECT_EQ(sm.getCurrentState(), State::Draining);
}

TEST_F(StateMachineTest, DrainingToCompletedTransition) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    sm.transition(EventType::SYS_WASH_COMPLETE);
    sm.transition(EventType::SYS_RINSE_COMPLETE);
    sm.transition(EventType::SYS_SPIN_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Draining);
    
    EXPECT_TRUE(sm.canTransition(EventType::SYS_DRAIN_COMPLETE));
    EXPECT_TRUE(sm.transition(EventType::SYS_DRAIN_COMPLETE));
    EXPECT_EQ(sm.getCurrentState(), State::Completed);
}

TEST_F(StateMachineTest, FullCycleTransition) {
    EXPECT_EQ(sm.getCurrentState(), State::Idle);
    
    sm.transition(EventType::CMD_SELECT_MODE);
    EXPECT_EQ(sm.getCurrentState(), State::Ready);
    
    sm.transition(EventType::CMD_START);
    EXPECT_EQ(sm.getCurrentState(), State::Filling);
    
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    EXPECT_EQ(sm.getCurrentState(), State::Washing);
    
    sm.transition(EventType::SYS_WASH_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Rinsing);
    
    sm.transition(EventType::SYS_RINSE_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Spinning);
    
    sm.transition(EventType::SYS_SPIN_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Draining);
    
    sm.transition(EventType::SYS_DRAIN_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Completed);
}

TEST_F(StateMachineTest, InvalidTransitionFromIdle) {
    EXPECT_FALSE(sm.canTransition(EventType::CMD_START));
    EXPECT_FALSE(sm.transition(EventType::CMD_START));
    EXPECT_EQ(sm.getCurrentState(), State::Idle);
}

TEST_F(StateMachineTest, InvalidTransitionFromDoorOpen) {
    sm.transition(EventType::CMD_OPEN_DOOR);
    EXPECT_EQ(sm.getCurrentState(), State::DoorOpen);
    
    EXPECT_FALSE(sm.canTransition(EventType::CMD_START));
    EXPECT_FALSE(sm.transition(EventType::CMD_START));
    EXPECT_EQ(sm.getCurrentState(), State::DoorOpen);
}

TEST_F(StateMachineTest, PauseFromWashingState) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    EXPECT_EQ(sm.getCurrentState(), State::Washing);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_PAUSE));
    EXPECT_TRUE(sm.transition(EventType::CMD_PAUSE));
    EXPECT_EQ(sm.getCurrentState(), State::Paused);
}

TEST_F(StateMachineTest, PauseFromFillingState) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    EXPECT_EQ(sm.getCurrentState(), State::Filling);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_PAUSE));
    EXPECT_TRUE(sm.transition(EventType::CMD_PAUSE));
    EXPECT_EQ(sm.getCurrentState(), State::Paused);
}

TEST_F(StateMachineTest, IsActiveStateCheck) {
    EXPECT_FALSE(sm.isActiveState());
    
    sm.transition(EventType::CMD_SELECT_MODE);
    EXPECT_FALSE(sm.isActiveState());
    
    sm.transition(EventType::CMD_START);
    EXPECT_TRUE(sm.isActiveState());
    
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    EXPECT_TRUE(sm.isActiveState());
}

TEST_F(StateMachineTest, IsSafeToOpenDoorCheck) {
    EXPECT_TRUE(sm.isSafeToOpenDoor());
    
    sm.transition(EventType::CMD_OPEN_DOOR);
    EXPECT_TRUE(sm.isSafeToOpenDoor());
    
    sm.transition(EventType::CMD_CLOSE_DOOR);
    sm.transition(EventType::CMD_SELECT_MODE);
    EXPECT_TRUE(sm.isSafeToOpenDoor());
    
    sm.transition(EventType::CMD_START);
    EXPECT_FALSE(sm.isSafeToOpenDoor());
}

TEST_F(StateMachineTest, ForceStateTransition) {
    sm.forceState(State::Washing);
    EXPECT_EQ(sm.getCurrentState(), State::Washing);
    
    sm.forceState(State::Idle);
    EXPECT_EQ(sm.getCurrentState(), State::Idle);
}

TEST_F(StateMachineTest, PreviousStateTracking) {
    sm.transition(EventType::CMD_SELECT_MODE);
    EXPECT_EQ(sm.getPreviousState(), State::Idle);
    
    sm.transition(EventType::CMD_START);
    EXPECT_EQ(sm.getPreviousState(), State::Ready);
}