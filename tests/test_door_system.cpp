#include <gtest/gtest.h>
#include "DoorSystem.hpp"
#include "Types.hpp"

class DoorSystemTest : public ::testing::Test {
protected:
    DoorSystem door;

    void SetUp() override {
        door.reset();
    }
};

TEST_F(DoorSystemTest, InitialStateIsOpen) {
    EXPECT_TRUE(door.isOpen());
    EXPECT_FALSE(door.isLocked());
}

TEST_F(DoorSystemTest, CloseDoor) {
    EXPECT_TRUE(door.closeDoor());
    EXPECT_FALSE(door.isOpen());
}

TEST_F(DoorSystemTest, OpenDoor) {
    door.closeDoor();
    EXPECT_FALSE(door.isOpen());
    
    EXPECT_TRUE(door.openDoor());
    EXPECT_TRUE(door.isOpen());
}

TEST_F(DoorSystemTest, LockDoorWhenClosed) {
    door.closeDoor();
    door.lock();
    EXPECT_TRUE(door.isLocked());
}

TEST_F(DoorSystemTest, CannotLockOpenDoor) {
    EXPECT_TRUE(door.isOpen());
    door.lock();
    EXPECT_FALSE(door.isLocked());
}

TEST_F(DoorSystemTest, CannotOpenLockedDoor) {
    door.closeDoor();
    door.lock();
    
    EXPECT_FALSE(door.openDoor());
    EXPECT_FALSE(door.isOpen());
}

TEST_F(DoorSystemTest, UnlockDoor) {
    door.closeDoor();
    door.lock();
    EXPECT_TRUE(door.isLocked());
    
    door.unlock();
    EXPECT_FALSE(door.isLocked());
}

TEST_F(DoorSystemTest, CanOpenAfterUnlock) {
    door.closeDoor();
    door.lock();
    door.unlock();
    
    EXPECT_TRUE(door.openDoor());
    EXPECT_TRUE(door.isOpen());
}

TEST_F(DoorSystemTest, CanOpenCheck) {
    door.closeDoor();
    EXPECT_TRUE(door.canOpen());
    
    door.lock();
    EXPECT_FALSE(door.canOpen());
    
    door.unlock();
    EXPECT_TRUE(door.canOpen());
}

TEST_F(DoorSystemTest, GetStatusOpen) {
    EXPECT_EQ(door.getStatus(), DoorStatus::Open);
}

TEST_F(DoorSystemTest, GetStatusClosedUnlocked) {
    door.closeDoor();
    EXPECT_EQ(door.getStatus(), DoorStatus::ClosedUnlocked);
}

TEST_F(DoorSystemTest, GetStatusClosedLocked) {
    door.closeDoor();
    door.lock();
    EXPECT_EQ(door.getStatus(), DoorStatus::ClosedLocked);
}

TEST_F(DoorSystemTest, ResetDoor) {
    door.closeDoor();
    door.lock();
    
    door.reset();
    
    EXPECT_TRUE(door.isOpen());
    EXPECT_FALSE(door.isLocked());
}

TEST_F(DoorSystemTest, MultipleOpenCloseOperations) {
    for (int i = 0; i < 5; i++) {
        door.closeDoor();
        EXPECT_FALSE(door.isOpen());
        
        door.openDoor();
        EXPECT_TRUE(door.isOpen());
    }
}

TEST_F(DoorSystemTest, LockUnlockCycle) {
    door.closeDoor();
    
    for (int i = 0; i < 3; i++) {
        door.lock();
        EXPECT_TRUE(door.isLocked());
        EXPECT_FALSE(door.canOpen());
        
        door.unlock();
        EXPECT_FALSE(door.isLocked());
        EXPECT_TRUE(door.canOpen());
    }
}