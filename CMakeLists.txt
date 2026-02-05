cmake_minimum_required(VERSION 3.16)
project(WashingMachineSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(LIB_SOURCES
    src/WashingMachine.cpp
    src/StateMachine.cpp
    src/EventEngine.cpp
    src/DoorSystem.cpp
    src/WaterSystem.cpp
    src/MotorSystem.cpp
    src/ConfigManager.cpp
    src/CLI.cpp
)

add_library(washing_machine_lib STATIC ${LIB_SOURCES})

add_executable(washing_machine src/main.cpp)
target_link_libraries(washing_machine PRIVATE washing_machine_lib)

file(COPY ${PROJECT_SOURCE_DIR}/config DESTINATION ${PROJECT_BINARY_DIR})

enable_testing()
add_subdirectory(tests)