#ifndef EVENT_HPP
#define EVENT_HPP

#include "Types.hpp"
#include <variant>
#include <chrono>
#include <string>

class Event {
private:
    EventType type;
    std::variant<std::monostate, int, float, std::string> data;
    std::chrono::steady_clock::time_point timestamp;

public:
    Event(EventType type)
        : type(type), data(std::monostate{}), timestamp(std::chrono::steady_clock::now()) {}

    Event(EventType type, int intData)
        : type(type), data(intData), timestamp(std::chrono::steady_clock::now()) {}

    Event(EventType type, float floatData)
        : type(type), data(floatData), timestamp(std::chrono::steady_clock::now()) {}

    Event(EventType type, const std::string& strData)
        : type(type), data(strData), timestamp(std::chrono::steady_clock::now()) {}

    EventType getType() const { return type; }

    template<typename T>
    T getData() const {
        return std::get<T>(data);
    }

    bool hasData() const {
        return !std::holds_alternative<std::monostate>(data);
    }

    auto getTimestamp() const { return timestamp; }

    std::string toString() const {
        return eventTypeToString(type);
    }
};

#endif