#pragma once

#include <sstream>
#include <iostream>
#include <ostream>
#include <mutex>

enum Level {
    ERROR,
    WARN,
    NORMAL,
};

class Logger;
class Filter {
public:
    explicit Filter(Level level) : level_(level) {};
    friend class Logger;

    // Non-copyable
    Filter(Filter&) = delete;
    Filter& operator=(Filter&) = delete;

    // Movable
    Filter(Filter&&) noexcept ;
    Filter& operator=(Filter&&) noexcept;

    ~Filter();

    template<typename T>
    friend Filter operator<<(Filter filter, const T message) {
        if (filter.level_ <= filter.current_) {
            filter.local_ << message;
        }

        return filter;
    };

    friend Filter operator<<(Filter filter, std::basic_ostream<char>& (*func)(std::basic_ostream<char>&)) {
        if (filter.level_ <= filter.current_) {
            filter.local_ << func;
        }

        return filter;
    };
    friend Filter operator<<(Logger& logger, Filter filter);
private:
    bool is_moved_{false};
    Level level_;
    Level current_{ERROR};
    std::ostringstream local_{};
    std::ostream* global_{nullptr};
};

class Logger {
public:
    static Logger* Instance();
    ~Logger(){ delete instance_; };

    // Non-copyable
    Logger(Logger&) = delete;
    Logger& operator=(Logger&) = delete;

    // Non-movable
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;


    void SetLevel(Level level) { current_ = level; };
    void SetOutput(std::ostream* stream) { output_ = stream; };



    friend Filter operator<<(Logger& logger, Filter filter) {
        filter.current_ = logger.current_;
        filter.global_ = logger.output_;

        return filter;
    }

private:
    Logger() = default;
private:
    static Logger* instance_;
    static std::mutex mtx_;

    Level current_{ERROR};
    std::ostream* output_{&std::cout};
};

Logger* Logger::instance_{nullptr};
std::mutex Logger::mtx_{};


Logger* Logger::Instance() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (instance_ == nullptr) {
        instance_ = new Logger;
    }

    return instance_;
}

Filter::Filter(Filter&& filter) noexcept : level_(filter.level_),
                                           current_(filter.current_), local_(std::move(filter.local_)), global_(filter.global_){ filter.is_moved_ = true; }

Filter& Filter::operator=(Filter&& filter) noexcept {
    level_ = filter.level_;
    current_ = filter.current_;
    local_ = std::move(filter.local_);
    global_ = filter.global_;

    filter.is_moved_ = true;
    return *this;
}

Filter::~Filter() {
    if (!is_moved_) {
        *global_ << local_.str();
    }
}
#define LogError *Logger::Instance() << Filter(ERROR) << "[ERROR] "
#define LogWarn *Logger::Instance() << Filter(WARN) << "[WARN] "
#define LogNormal *Logger::Instance() << Filter(NORMAL) << "[NORMAL] "
