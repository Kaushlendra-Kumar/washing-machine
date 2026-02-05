#ifndef WASH_MODE_HPP
#define WASH_MODE_HPP

#include <string>

struct WashMode {
    std::string name;
    int durationMinutes;
    int spinSpeedRPM;
    float waterLevelLiters;
    int temperatureCelsius;

    WashMode()
        : name("Default"), durationMinutes(30), spinSpeedRPM(800),
          waterLevelLiters(30.0f), temperatureCelsius(40) {}

    WashMode(const std::string& name, int duration, int spinSpeed,
             float waterLevel, int temperature)
        : name(name), durationMinutes(duration), spinSpeedRPM(spinSpeed),
          waterLevelLiters(waterLevel), temperatureCelsius(temperature) {}

    int getAdjustedDuration(float loadKg) const {
        return durationMinutes + static_cast<int>(loadKg * 2);
    }

    float getAdjustedWaterLevel(float loadKg) const {
        float adjusted = waterLevelLiters + (loadKg * 3.0f);
        return (adjusted > 50.0f) ? 50.0f : adjusted;
    }
};

#endif