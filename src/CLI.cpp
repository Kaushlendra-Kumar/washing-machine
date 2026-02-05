#include "CLI.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

CLI::CLI(WashingMachine& machine) : machine(machine), running(false) {}

void CLI::printWelcome() {
    std::cout << "\n";
    std::cout << "+------------------------------------------------------------+\n";
    std::cout << "|         WASHING MACHINE SIMULATOR v1.0.0                   |\n";
    std::cout << "|                                                            |\n";
    std::cout << "|  A C++ simulation of a consumer washing machine            |\n";
    std::cout << "+------------------------------------------------------------+\n";
    std::cout << "\n";
    std::cout << "Type 'help' for available commands.\n\n";
}

void CLI::printHelp() {
    std::cout << "\n";
    std::cout << "+------------------------------------------------------------+\n";
    std::cout << "|                    AVAILABLE COMMANDS                      |\n";
    std::cout << "+------------------------------------------------------------+\n";
    std::cout << "|  Door Control:                                             |\n";
    std::cout << "|    open        - Open the door                             |\n";
    std::cout << "|    close       - Close the door                            |\n";
    std::cout << "|                                                            |\n";
    std::cout << "|  Load & Mode:                                              |\n";
    std::cout << "|    load <kg>   - Set load weight (0-6 kg)                  |\n";
    std::cout << "|    mode <1-4>  - Select wash mode                          |\n";
    std::cout << "|    modes       - Show available modes                      |\n";
    std::cout << "|                                                            |\n";
    std::cout << "|  Cycle Control:                                            |\n";
    std::cout << "|    start       - Start wash cycle                          |\n";
    std::cout << "|    pause       - Pause current cycle                       |\n";
    std::cout << "|    resume      - Resume paused cycle                       |\n";
    std::cout << "|    stop        - Stop/cancel cycle                         |\n";
    std::cout << "|    emergency   - Emergency stop (immediate)                |\n";
    std::cout << "|                                                            |\n";
    std::cout << "|  Information:                                              |\n";
    std::cout << "|    status      - Show current status                       |\n";
    std::cout << "|    help        - Show this help message                    |\n";
    std::cout << "|    clear       - Clear screen                              |\n";
    std::cout << "|    exit/quit   - Exit simulator                            |\n";
    std::cout << "+------------------------------------------------------------+\n";
    std::cout << "\n";
}

void CLI::printStatus() {
    SystemStatus status = machine.getStatus();

    std::cout << "\n";
    std::cout << "+------------------------------------------------------------+\n";
    std::cout << "|                    MACHINE STATUS                          |\n";
    std::cout << "+------------------------------------------------------------+\n";

    std::cout << "|  State:         " << std::left << std::setw(42)
              << stateToString(status.state) << "|\n";

    std::cout << "|  Door:          " << std::left << std::setw(42)
              << doorStatusToString(status.doorStatus) << "|\n";

    std::cout << "|  Mode:          " << std::left << std::setw(42)
              << status.modeName << "|\n";

    std::ostringstream loadStr;
    loadStr << std::fixed << std::setprecision(1) << status.loadKg << " kg";
    std::cout << "|  Load:          " << std::left << std::setw(42)
              << loadStr.str() << "|\n";

    std::ostringstream waterStr;
    waterStr << std::fixed << std::setprecision(1) << status.waterLevel << " / "
             << status.targetWaterLevel << " L";
    std::cout << "|  Water Level:   " << std::left << std::setw(42)
              << waterStr.str() << "|\n";

    std::ostringstream motorStr;
    motorStr << status.motorRPM << " RPM";
    std::cout << "|  Motor Speed:   " << std::left << std::setw(42)
              << motorStr.str() << "|\n";

    std::ostringstream progressStr;
    progressStr << std::fixed << std::setprecision(1) << status.progressPercent << "%";
    std::cout << "|  Progress:      " << std::left << std::setw(42)
              << progressStr.str() << "|\n";

    int mins = status.remainingSeconds / 60;
    int secs = status.remainingSeconds % 60;
    std::ostringstream timeStr;
    timeStr << std::setfill('0') << std::setw(2) << mins << ":"
            << std::setfill('0') << std::setw(2) << secs;
    std::cout << "|  Time Left:     " << std::left << std::setw(42)
              << timeStr.str() << "|\n";

    if (status.fault != FaultCode::None) {
        std::cout << "|  FAULT:         " << std::left << std::setw(42)
                  << faultCodeToString(status.fault) << "|\n";
    }

    std::cout << "+------------------------------------------------------------+\n";
    std::cout << "\n";
}

void CLI::printModes() {
    machine.getConfigManager().printModes();
}

std::vector<std::string> CLI::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token) {
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
        tokens.push_back(token);
    }

    return tokens;
}

void CLI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool CLI::parseCommand(const std::string& input) {
    std::vector<std::string> tokens = tokenize(input);

    if (tokens.empty()) {
        return true;
    }

    const std::string& cmd = tokens[0];

    if (cmd == "exit" || cmd == "quit") {
        return false;
    }

    if (cmd == "help") {
        printHelp();
    }
    else if (cmd == "status") {
        printStatus();
    }
    else if (cmd == "modes") {
        printModes();
    }
    else if (cmd == "clear" || cmd == "cls") {
        clearScreen();
    }
    else if (cmd == "open") {
        machine.openDoor();
    }
    else if (cmd == "close") {
        machine.closeDoor();
    }
    else if (cmd == "load") {
        if (tokens.size() < 2) {
            std::cout << "Usage: load <kg>\n";
        } else {
            try {
                float kg = std::stof(tokens[1]);
                machine.setLoad(kg);
            } catch (...) {
                std::cout << "Invalid load value.\n";
            }
        }
    }
    else if (cmd == "mode") {
        if (tokens.size() < 2) {
            std::cout << "Usage: mode <1-4>\n";
            printModes();
        } else {
            try {
                int modeNum = std::stoi(tokens[1]);
                machine.selectMode(modeNum - 1);
            } catch (...) {
                std::cout << "Invalid mode number.\n";
            }
        }
    }
    else if (cmd == "start") {
        machine.start();
    }
    else if (cmd == "pause") {
        machine.pause();
    }
    else if (cmd == "resume") {
        machine.resume();
    }
    else if (cmd == "stop") {
        machine.stop();
    }
    else if (cmd == "emergency") {
        machine.emergencyStop();
    }
    else {
        std::cout << "Unknown command: " << cmd << ". Type 'help' for commands.\n";
    }

    return true;
}

void CLI::start() {
    running = true;
    printWelcome();

    std::string input;

    while (running && machine.isRunning()) {
        std::cout << "washing-machine> ";
        std::getline(std::cin, input);

        if (std::cin.eof()) {
            break;
        }

        if (!parseCommand(input)) {
            running = false;
        }
    }

    std::cout << "Shutting down simulator...\n";
}

void CLI::stop() {
    running = false;
}

bool CLI::isRunning() const {
    return running;
}