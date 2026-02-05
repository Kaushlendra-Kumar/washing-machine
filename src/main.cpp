#include "WashingMachine.hpp"
#include "CLI.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string configPath = "config/wash_modes.json";

    if (argc > 1) {
        configPath = argv[1];
    }

    WashingMachine machine;

    if (!machine.initialize(configPath)) {
        std::cerr << "Failed to initialize washing machine.\n";
        return 1;
    }

    machine.run();

    CLI cli(machine);
    cli.start();

    machine.shutdown();

    std::cout << "Goodbye!\n";
    return 0;
}