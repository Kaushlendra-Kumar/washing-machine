#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

ConfigManager::ConfigManager() {
    loadDefaultConfig();
}

bool ConfigManager::parseJsonFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    modes.clear();

    size_t pos = 0;
    while ((pos = content.find("\"name\"", pos)) != std::string::npos) {
        WashMode mode;

        size_t nameStart = content.find("\"", pos + 6) + 1;
        size_t nameEnd = content.find("\"", nameStart);
        mode.name = content.substr(nameStart, nameEnd - nameStart);

        size_t durationPos = content.find("\"duration_minutes\"", pos);
        if (durationPos != std::string::npos) {
            size_t colonPos = content.find(":", durationPos);
            size_t commaPos = content.find(",", colonPos);
            std::string value = content.substr(colonPos + 1, commaPos - colonPos - 1);
            mode.durationMinutes = std::stoi(value);
        }

        size_t spinPos = content.find("\"spin_speed_rpm\"", pos);
        if (spinPos != std::string::npos) {
            size_t colonPos = content.find(":", spinPos);
            size_t commaPos = content.find(",", colonPos);
            std::string value = content.substr(colonPos + 1, commaPos - colonPos - 1);
            mode.spinSpeedRPM = std::stoi(value);
        }

        size_t waterPos = content.find("\"water_level_liters\"", pos);
        if (waterPos != std::string::npos) {
            size_t colonPos = content.find(":", waterPos);
            size_t commaPos = content.find(",", colonPos);
            std::string value = content.substr(colonPos + 1, commaPos - colonPos - 1);
            mode.waterLevelLiters = std::stof(value);
        }

        size_t tempPos = content.find("\"temperature_celsius\"", pos);
        if (tempPos != std::string::npos) {
            size_t colonPos = content.find(":", tempPos);
            size_t endPos = content.find_first_of(",}", colonPos);
            std::string value = content.substr(colonPos + 1, endPos - colonPos - 1);
            mode.temperatureCelsius = std::stoi(value);
        }

        modes.push_back(mode);
        pos = nameEnd;
    }

    return !modes.empty();
}

bool ConfigManager::loadConfig(const std::string& path) {
    configPath = path;
    if (parseJsonFile(path)) {
        return true;
    }
    loadDefaultConfig();
    return false;
}

void ConfigManager::loadDefaultConfig() {
    modes.clear();
    modes.push_back(WashMode("Quick Wash", 15, 800, 20.0f, 30));
    modes.push_back(WashMode("Normal", 45, 1000, 35.0f, 40));
    modes.push_back(WashMode("Heavy", 60, 1200, 45.0f, 60));
    modes.push_back(WashMode("Delicate", 30, 400, 30.0f, 30));
}

const WashMode& ConfigManager::getMode(int index) const {
    if (index < 0 || index >= static_cast<int>(modes.size())) {
        return modes[0];
    }
    return modes[index];
}

int ConfigManager::getModeCount() const {
    return static_cast<int>(modes.size());
}

const std::vector<WashMode>& ConfigManager::getAllModes() const {
    return modes;
}

void ConfigManager::printModes() const {
    std::cout << "\nAvailable Wash Modes:\n";
    std::cout << "---------------------\n";
    for (int i = 0; i < static_cast<int>(modes.size()); ++i) {
        const auto& mode = modes[i];
        std::cout << "  " << (i + 1) << ". " << mode.name << "\n";
        std::cout << "     Duration: " << mode.durationMinutes << " min\n";
        std::cout << "     Spin: " << mode.spinSpeedRPM << " RPM\n";
        std::cout << "     Water: " << mode.waterLevelLiters << " L\n";
        std::cout << "     Temp: " << mode.temperatureCelsius << " C\n";
    }
    std::cout << std::endl;
}