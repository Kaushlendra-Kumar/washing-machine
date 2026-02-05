#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include "WashMode.hpp"
#include <vector>
#include <string>

class ConfigManager {
private:
    std::vector<WashMode> modes;
    std::string configPath;

    bool parseJsonFile(const std::string& path);

public:
    ConfigManager();

    bool loadConfig(const std::string& path);
    void loadDefaultConfig();

    const WashMode& getMode(int index) const;
    int getModeCount() const;
    const std::vector<WashMode>& getAllModes() const;

    void printModes() const;
};

#endif