#ifndef EVENT_ENGINE_HPP
#define EVENT_ENGINE_HPP

#include "Event.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <optional>

class EventEngine {
private:
    std::queue<Event> eventQueue;
    mutable std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::function<void(const Event&)> eventHandler;

public:
    EventEngine();
    ~EventEngine();

    void setEventHandler(std::function<void(const Event&)> handler);
    void pushEvent(const Event& event);
    void pushEvent(EventType type);
    void pushEvent(EventType type, int data);
    void pushEvent(EventType type, float data);

    std::optional<Event> popEvent();
    std::optional<Event> waitForEvent();
    bool hasEvents() const;
    size_t getQueueSize() const;
    void clear();

    void start();
    void stop();
    bool isRunning() const;
};

#endif