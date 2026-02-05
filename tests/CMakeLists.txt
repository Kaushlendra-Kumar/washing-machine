set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/external/googletest ${CMAKE_BINARY_DIR}/googletest)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(unit_tests
    test_state_machine.cpp
    test_door_system.cpp
    test_water_system.cpp
    test_emergency.cpp
    test_safety_interlocks.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    washing_machine_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)