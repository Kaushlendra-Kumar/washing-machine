#ifndef CLI_HPP
#define CLI_HPP

#include "WashingMachine.hpp"
#include <atomic>
#include <string>
#include <vector>

class CLI {
private:
    WashingMachine& machine;
    std::atomic<bool> running;

    void printWelcome();
    void printHelp();
    void printStatus();
    void printModes();

    bool parseCommand(const std::string& input);
    std::vector<std::string> tokenize(const std::string& input);
    void clearScreen();

public:
    CLI(WashingMachine& machine);

    void start();
    void stop();
    bool isRunning() const;
};

#endif