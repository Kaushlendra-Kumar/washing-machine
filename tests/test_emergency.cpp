#include <gtest/gtest.h>
#include "StateMachine.hpp"
#include "MotorSystem.hpp"
#include "WaterSystem.hpp"
#include "DoorSystem.hpp"
#include "Types.hpp"

class EmergencyStopTest : public ::testing::Test {
protected:
    StateMachine sm;
    MotorSystem motor;
    WaterSystem water;
    DoorSystem door;

    void SetUp() override {
        sm.reset();
        motor.reset();
        water.reset();
        door.reset();
    }

    void setMachineToWashingState() {
        sm.transition(EventType::CMD_SELECT_MODE);
        sm.transition(EventType::CMD_START);
        sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    }
};

TEST_F(EmergencyStopTest, EmergencyFromFillingState) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    EXPECT_EQ(sm.getCurrentState(), State::Filling);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_EMERGENCY));
    EXPECT_TRUE(sm.transition(EventType::CMD_EMERGENCY));
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
}

TEST_F(EmergencyStopTest, EmergencyFromWashingState) {
    setMachineToWashingState();
    EXPECT_EQ(sm.getCurrentState(), State::Washing);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_EMERGENCY));
    EXPECT_TRUE(sm.transition(EventType::CMD_EMERGENCY));
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
}

TEST_F(EmergencyStopTest, EmergencyFromRinsingState) {
    setMachineToWashingState();
    sm.transition(EventType::SYS_WASH_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Rinsing);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_EMERGENCY));
    EXPECT_TRUE(sm.transition(EventType::CMD_EMERGENCY));
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
}

TEST_F(EmergencyStopTest, EmergencyFromSpinningState) {
    setMachineToWashingState();
    sm.transition(EventType::SYS_WASH_COMPLETE);
    sm.transition(EventType::SYS_RINSE_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Spinning);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_EMERGENCY));
    EXPECT_TRUE(sm.transition(EventType::CMD_EMERGENCY));
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
}

TEST_F(EmergencyStopTest, EmergencyFromDrainingState) {
    setMachineToWashingState();
    sm.transition(EventType::SYS_WASH_COMPLETE);
    sm.transition(EventType::SYS_RINSE_COMPLETE);
    sm.transition(EventType::SYS_SPIN_COMPLETE);
    EXPECT_EQ(sm.getCurrentState(), State::Draining);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_EMERGENCY));
    EXPECT_TRUE(sm.transition(EventType::CMD_EMERGENCY));
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
}

TEST_F(EmergencyStopTest, EmergencyFromPausedState) {
    setMachineToWashingState();
    sm.transition(EventType::CMD_PAUSE);
    EXPECT_EQ(sm.getCurrentState(), State::Paused);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_EMERGENCY));
    EXPECT_TRUE(sm.transition(EventType::CMD_EMERGENCY));
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
}

TEST_F(EmergencyStopTest, MotorEmergencyStop) {
    motor.start(1000, Direction::Clockwise);
    for (int i = 0; i < 10; i++) {
        motor.update(0.1f);
    }
    EXPECT_TRUE(motor.isRunning());
    EXPECT_GT(motor.getCurrentRPM(), 0);
    
    motor.emergencyStop();
    
    EXPECT_FALSE(motor.isRunning());
    EXPECT_EQ(motor.getCurrentRPM(), 0);
    EXPECT_EQ(motor.getDirection(), Direction::Stopped);
}

TEST_F(EmergencyStopTest, MotorEmergencyStopImmediate) {
    motor.start(1200, Direction::Clockwise);
    for (int i = 0; i < 50; i++) {
        motor.update(0.1f);
    }
    
    int rpmBefore = motor.getCurrentRPM();
    EXPECT_GT(rpmBefore, 0);
    
    motor.emergencyStop();
    
    EXPECT_EQ(motor.getCurrentRPM(), 0);
}

TEST_F(EmergencyStopTest, WaterStopsDuringEmergency) {
    water.startFilling(30.0f);
    EXPECT_TRUE(water.isFilling());
    
    water.stopFilling();
    EXPECT_FALSE(water.isFilling());
}

TEST_F(EmergencyStopTest, WaterDrainsAfterEmergency) {
    water.startFilling(20.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    float waterLevel = water.getCurrentLevel();
    EXPECT_GT(waterLevel, 0);
    
    water.stopFilling();
    water.startDraining();
    
    EXPECT_TRUE(water.isDraining());
    
    for (int i = 0; i < 30; i++) {
        water.update(0.5f);
    }
    
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 0.0f);
}

TEST_F(EmergencyStopTest, DoorUnlocksAfterEmergencySafe) {
    door.closeDoor();
    door.lock();
    EXPECT_TRUE(door.isLocked());
    EXPECT_FALSE(door.canOpen());
    
    door.unlock();
    
    EXPECT_FALSE(door.isLocked());
    EXPECT_TRUE(door.canOpen());
}

TEST_F(EmergencyStopTest, EmergencyStopToIdleTransition) {
    setMachineToWashingState();
    sm.transition(EventType::CMD_EMERGENCY);
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
    
    EXPECT_TRUE(sm.canTransition(EventType::CMD_STOP));
    EXPECT_TRUE(sm.transition(EventType::CMD_STOP));
    EXPECT_EQ(sm.getCurrentState(), State::Idle);
}

TEST_F(EmergencyStopTest, CannotStartFromEmergencyStop) {
    setMachineToWashingState();
    sm.transition(EventType::CMD_EMERGENCY);
    
    EXPECT_FALSE(sm.canTransition(EventType::CMD_START));
    EXPECT_FALSE(sm.transition(EventType::CMD_START));
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
}

TEST_F(EmergencyStopTest, CannotOpenDoorFromEmergencyStop) {
    setMachineToWashingState();
    sm.transition(EventType::CMD_EMERGENCY);
    
    EXPECT_FALSE(sm.canTransition(EventType::CMD_OPEN_DOOR));
}

TEST_F(EmergencyStopTest, FullEmergencySequence) {
    door.closeDoor();
    door.lock();
    
    motor.start(1000, Direction::Clockwise);
    for (int i = 0; i < 10; i++) {
        motor.update(0.1f);
    }
    
    water.startFilling(30.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    setMachineToWashingState();
    sm.transition(EventType::CMD_EMERGENCY);
    
    motor.emergencyStop();
    water.stopFilling();
    water.startDraining();
    
    EXPECT_EQ(sm.getCurrentState(), State::EmergencyStop);
    EXPECT_EQ(motor.getCurrentRPM(), 0);
    EXPECT_FALSE(water.isFilling());
    EXPECT_TRUE(water.isDraining());
    
    for (int i = 0; i < 50; i++) {
        water.update(0.5f);
    }
    
    door.unlock();
    
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 0.0f);
    EXPECT_TRUE(door.canOpen());
}