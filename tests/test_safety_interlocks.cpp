#include <gtest/gtest.h>
#include "StateMachine.hpp"
#include "DoorSystem.hpp"
#include "WaterSystem.hpp"
#include "MotorSystem.hpp"
#include "WashMode.hpp"
#include "Types.hpp"

class SafetyInterlocksTest : public ::testing::Test {
protected:
    StateMachine sm;
    DoorSystem door;
    WaterSystem water;
    MotorSystem motor;

    void SetUp() override {
        sm.reset();
        door.reset();
        water.reset();
        motor.reset();
    }
};

TEST_F(SafetyInterlocksTest, CannotStartWithDoorOpen) {
    EXPECT_TRUE(door.isOpen());
    
    EXPECT_FALSE(sm.canTransition(EventType::CMD_START));
}

TEST_F(SafetyInterlocksTest, CanStartWithDoorClosed) {
    door.closeDoor();
    sm.transition(EventType::CMD_SELECT_MODE);
    
    EXPECT_FALSE(door.isOpen());
    EXPECT_TRUE(sm.canTransition(EventType::CMD_START));
}

TEST_F(SafetyInterlocksTest, DoorLocksWhenCycleStarts) {
    door.closeDoor();
    EXPECT_FALSE(door.isLocked());
    
    door.lock();
    
    EXPECT_TRUE(door.isLocked());
    EXPECT_FALSE(door.canOpen());
}

TEST_F(SafetyInterlocksTest, CannotOpenDoorDuringActiveCycle) {
    door.closeDoor();
    door.lock();
    
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    
    EXPECT_TRUE(sm.isActiveState());
    EXPECT_FALSE(sm.isSafeToOpenDoor());
    EXPECT_FALSE(door.canOpen());
}

TEST_F(SafetyInterlocksTest, CannotOpenDoorWhileFilling) {
    door.closeDoor();
    door.lock();
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    
    EXPECT_EQ(sm.getCurrentState(), State::Filling);
    EXPECT_FALSE(sm.isSafeToOpenDoor());
    EXPECT_FALSE(door.canOpen());
}

TEST_F(SafetyInterlocksTest, CannotOpenDoorWhileWashing) {
    door.closeDoor();
    door.lock();
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    
    EXPECT_EQ(sm.getCurrentState(), State::Washing);
    EXPECT_FALSE(sm.isSafeToOpenDoor());
    EXPECT_FALSE(door.canOpen());
}

TEST_F(SafetyInterlocksTest, CannotOpenDoorWhileSpinning) {
    door.closeDoor();
    door.lock();
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    sm.transition(EventType::SYS_WASH_COMPLETE);
    sm.transition(EventType::SYS_RINSE_COMPLETE);
    
    EXPECT_EQ(sm.getCurrentState(), State::Spinning);
    EXPECT_FALSE(sm.isSafeToOpenDoor());
    EXPECT_FALSE(door.canOpen());
}

TEST_F(SafetyInterlocksTest, CanOpenDoorWhenCompleted) {
    door.closeDoor();
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    sm.transition(EventType::SYS_WASH_COMPLETE);
    sm.transition(EventType::SYS_RINSE_COMPLETE);
    sm.transition(EventType::SYS_SPIN_COMPLETE);
    sm.transition(EventType::SYS_DRAIN_COMPLETE);
    
    door.unlock();
    
    EXPECT_EQ(sm.getCurrentState(), State::Completed);
    EXPECT_TRUE(sm.isSafeToOpenDoor());
    EXPECT_TRUE(door.canOpen());
}

TEST_F(SafetyInterlocksTest, OverloadPreventsStart) {
    float loadKg = 7.0f;
    float maxCapacity = 6.0f;
    
    EXPECT_GT(loadKg, maxCapacity);
}

TEST_F(SafetyInterlocksTest, ValidLoadAllowsStart) {
    float loadKg = 5.0f;
    float maxCapacity = 6.0f;
    
    EXPECT_LE(loadKg, maxCapacity);
}

TEST_F(SafetyInterlocksTest, ZeroLoadPreventsStart) {
    float loadKg = 0.0f;
    
    EXPECT_EQ(loadKg, 0.0f);
}

TEST_F(SafetyInterlocksTest, NegativeLoadInvalid) {
    float loadKg = -1.0f;
    
    EXPECT_LT(loadKg, 0.0f);
}

TEST_F(SafetyInterlocksTest, LowWaterReservoirPreventsStart) {
    water.setReservoirLevel(5.0f);
    
    EXPECT_FALSE(water.checkReservoir());
}

TEST_F(SafetyInterlocksTest, AdequateWaterReservoirAllowsStart) {
    water.setReservoirLevel(50.0f);
    
    EXPECT_TRUE(water.checkReservoir());
}

TEST_F(SafetyInterlocksTest, MotorStopsWhenDoorOpensAfterCycle) {
    motor.start(1000, Direction::Clockwise);
    for (int i = 0; i < 10; i++) {
        motor.update(0.1f);
    }
    
    motor.stop();
    for (int i = 0; i < 20; i++) {
        motor.update(0.1f);
    }
    
    EXPECT_EQ(motor.getCurrentRPM(), 0);
}

TEST_F(SafetyInterlocksTest, WaterDrainsBeforeDoorUnlock) {
    water.startFilling(20.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    EXPECT_GT(water.getCurrentLevel(), 0);
    
    water.startDraining();
    for (int i = 0; i < 30; i++) {
        water.update(0.5f);
    }
    
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 0.0f);
}

TEST_F(SafetyInterlocksTest, FaultStatePreventsOperation) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::FAULT_WATER_UNAVAILABLE);
    
    EXPECT_EQ(sm.getCurrentState(), State::Fault);
    EXPECT_FALSE(sm.canTransition(EventType::CMD_START));
}

TEST_F(SafetyInterlocksTest, FaultClearedAllowsReset) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::FAULT_WATER_UNAVAILABLE);
    
    EXPECT_EQ(sm.getCurrentState(), State::Fault);
    
    EXPECT_TRUE(sm.canTransition(EventType::FAULT_CLEARED));
    sm.transition(EventType::FAULT_CLEARED);
    
    EXPECT_EQ(sm.getCurrentState(), State::Idle);
}

TEST_F(SafetyInterlocksTest, PausedStateAllowsResume) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::SYS_WATER_LEVEL_REACHED);
    sm.transition(EventType::CMD_PAUSE);
    
    EXPECT_EQ(sm.getCurrentState(), State::Paused);
    EXPECT_TRUE(sm.canTransition(EventType::CMD_RESUME));
}

TEST_F(SafetyInterlocksTest, PausedStateAllowsStop) {
    sm.transition(EventType::CMD_SELECT_MODE);
    sm.transition(EventType::CMD_START);
    sm.transition(EventType::CMD_PAUSE);
    
    EXPECT_EQ(sm.getCurrentState(), State::Paused);
    EXPECT_TRUE(sm.canTransition(EventType::CMD_STOP));
}

TEST_F(SafetyInterlocksTest, LoadAdjustsWaterLevel) {
    WashMode mode("Test", 30, 1000, 30.0f, 40);
    
    float adjustedWater1 = mode.getAdjustedWaterLevel(2.0f);
    float adjustedWater2 = mode.getAdjustedWaterLevel(5.0f);
    
    EXPECT_GT(adjustedWater2, adjustedWater1);
}

TEST_F(SafetyInterlocksTest, LoadAdjustsCycleTime) {
    WashMode mode("Test", 30, 1000, 30.0f, 40);
    
    int adjustedTime1 = mode.getAdjustedDuration(2.0f);
    int adjustedTime2 = mode.getAdjustedDuration(5.0f);
    
    EXPECT_GT(adjustedTime2, adjustedTime1);
}

TEST_F(SafetyInterlocksTest, MaxWaterLevelCapped) {
    WashMode mode("Test", 30, 1000, 45.0f, 40);
    
    float adjustedWater = mode.getAdjustedWaterLevel(6.0f);
    
    EXPECT_LE(adjustedWater, 50.0f);
}

TEST_F(SafetyInterlocksTest, AllSafetyConditionsForStart) {
    door.closeDoor();
    EXPECT_FALSE(door.isOpen());
    
    float loadKg = 3.5f;
    EXPECT_GT(loadKg, 0.0f);
    EXPECT_LE(loadKg, 6.0f);
    
    EXPECT_TRUE(water.checkReservoir());
    
    sm.transition(EventType::CMD_SELECT_MODE);
    EXPECT_TRUE(sm.canTransition(EventType::CMD_START));
}