#include <gtest/gtest.h>
#include "WaterSystem.hpp"
#include "Types.hpp"

class WaterSystemTest : public ::testing::Test {
protected:
    WaterSystem water;

    void SetUp() override {
        water.reset();
    }
};

TEST_F(WaterSystemTest, InitialState) {
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 0.0f);
    EXPECT_FALSE(water.isFilling());
    EXPECT_FALSE(water.isDraining());
}

TEST_F(WaterSystemTest, ReservoirInitiallyFull) {
    EXPECT_FLOAT_EQ(water.getReservoirLevel(), 100.0f);
    EXPECT_TRUE(water.checkReservoir());
}

TEST_F(WaterSystemTest, StartFilling) {
    water.startFilling(30.0f);
    EXPECT_TRUE(water.isFilling());
    EXPECT_FLOAT_EQ(water.getTargetLevel(), 30.0f);
}

TEST_F(WaterSystemTest, StopFilling) {
    water.startFilling(30.0f);
    water.stopFilling();
    EXPECT_FALSE(water.isFilling());
}

TEST_F(WaterSystemTest, FillingIncreasesWaterLevel) {
    water.startFilling(30.0f);
    
    float initialLevel = water.getCurrentLevel();
    water.update(1.0f);
    
    EXPECT_GT(water.getCurrentLevel(), initialLevel);
}

TEST_F(WaterSystemTest, FillingStopsAtTargetLevel) {
    water.startFilling(10.0f);
    
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 10.0f);
    EXPECT_FALSE(water.isFilling());
}

TEST_F(WaterSystemTest, StartDraining) {
    water.startFilling(20.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    water.startDraining();
    EXPECT_TRUE(water.isDraining());
}

TEST_F(WaterSystemTest, DrainingDecreasesWaterLevel) {
    water.startFilling(20.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    float levelBefore = water.getCurrentLevel();
    water.startDraining();
    water.update(1.0f);
    
    EXPECT_LT(water.getCurrentLevel(), levelBefore);
}

TEST_F(WaterSystemTest, DrainingStopsAtZero) {
    water.startFilling(10.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    water.startDraining();
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 0.0f);
    EXPECT_FALSE(water.isDraining());
}

TEST_F(WaterSystemTest, ReservoirDecreasesWhileFilling) {
    float initialReservoir = water.getReservoirLevel();
    
    water.startFilling(20.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    EXPECT_LT(water.getReservoirLevel(), initialReservoir);
}

TEST_F(WaterSystemTest, AutoReplenish) {
    water.setReservoirLevel(5.0f);
    EXPECT_FALSE(water.checkReservoir());
    
    EXPECT_TRUE(water.autoReplenish());
    EXPECT_FLOAT_EQ(water.getReservoirLevel(), 100.0f);
    EXPECT_TRUE(water.checkReservoir());
}

TEST_F(WaterSystemTest, SetReservoirLevel) {
    water.setReservoirLevel(50.0f);
    EXPECT_FLOAT_EQ(water.getReservoirLevel(), 50.0f);
}

TEST_F(WaterSystemTest, SetReservoirLevelClampsToMax) {
    water.setReservoirLevel(200.0f);
    EXPECT_FLOAT_EQ(water.getReservoirLevel(), 100.0f);
}

TEST_F(WaterSystemTest, SetReservoirLevelClampsToMin) {
    water.setReservoirLevel(-10.0f);
    EXPECT_FLOAT_EQ(water.getReservoirLevel(), 0.0f);
}

TEST_F(WaterSystemTest, ResetWaterSystem) {
    water.startFilling(30.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    water.reset();
    
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 0.0f);
    EXPECT_FLOAT_EQ(water.getTargetLevel(), 0.0f);
    EXPECT_FALSE(water.isFilling());
    EXPECT_FALSE(water.isDraining());
    EXPECT_FLOAT_EQ(water.getReservoirLevel(), 100.0f);
}

TEST_F(WaterSystemTest, LowReservoirThreshold) {
    water.setReservoirLevel(9.0f);
    EXPECT_FALSE(water.checkReservoir());
    
    water.setReservoirLevel(11.0f);
    EXPECT_TRUE(water.checkReservoir());
}

TEST_F(WaterSystemTest, FillDrainCycle) {
    water.startFilling(15.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 15.0f);
    
    water.startDraining();
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 0.0f);
    
    water.startFilling(20.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    EXPECT_FLOAT_EQ(water.getCurrentLevel(), 20.0f);
}

TEST_F(WaterSystemTest, EventCallbackOnWaterLevelReached) {
    bool eventFired = false;
    EventType firedEvent;
    
    water.setEventCallback([&](EventType type) {
        eventFired = true;
        firedEvent = type;
    });
    
    water.startFilling(5.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    EXPECT_TRUE(eventFired);
    EXPECT_EQ(firedEvent, EventType::SYS_WATER_LEVEL_REACHED);
}

TEST_F(WaterSystemTest, EventCallbackOnDrainComplete) {
    bool eventFired = false;
    EventType firedEvent;
    
    water.setEventCallback([&](EventType type) {
        firedEvent = type;
        if (type == EventType::SYS_DRAIN_COMPLETE) {
            eventFired = true;
        }
    });
    
    water.startFilling(5.0f);
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    water.startDraining();
    for (int i = 0; i < 20; i++) {
        water.update(0.5f);
    }
    
    EXPECT_TRUE(eventFired);
    EXPECT_EQ(firedEvent, EventType::SYS_DRAIN_COMPLETE);
}