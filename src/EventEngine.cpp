#include "EventEngine.hpp"

EventEngine::EventEngine() : running(false), eventHandler(nullptr) {}

EventEngine::~EventEngine() {
    stop();
}

void EventEngine::setEventHandler(std::function<void(const Event&)> handler) {
    eventHandler = handler;
}

void EventEngine::pushEvent(const Event& event) {
    std::lock_guard<std::mutex> lock(queueMutex);
    eventQueue.push(event);
    cv.notify_one();
}

void EventEngine::pushEvent(EventType type) {
    pushEvent(Event(type));
}

void EventEngine::pushEvent(EventType type, int data) {
    pushEvent(Event(type, data));
}

void EventEngine::pushEvent(EventType type, float data) {
    pushEvent(Event(type, data));
}

std::optional<Event> EventEngine::popEvent() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (eventQueue.empty()) {
        return std::nullopt;
    }
    Event event = eventQueue.front();
    eventQueue.pop();
    return event;
}

std::optional<Event> EventEngine::waitForEvent() {
    std::unique_lock<std::mutex> lock(queueMutex);
    cv.wait(lock, [this] { return !eventQueue.empty() || !running; });

    if (!running && eventQueue.empty()) {
        return std::nullopt;
    }

    Event event = eventQueue.front();
    eventQueue.pop();
    return event;
}

bool EventEngine::hasEvents() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return !eventQueue.empty();
}

size_t EventEngine::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return eventQueue.size();
}

void EventEngine::clear() {
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
}

void EventEngine::start() {
    running = true;
}

void EventEngine::stop() {
    running = false;
    cv.notify_all();
}

bool EventEngine::isRunning() const {
    return running;
}