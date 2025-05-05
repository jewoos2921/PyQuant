#pragma once

#include <string>
#include <fstream>
#include <cstdio>

#include "macros.hpp"
#include "lf_queue.hpp"
#include "thread_utils.hpp"
#include "time_utils.hpp"

namespace QuantCpp
{
    namespace Common
    {
        constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;
        enum class LogType : int8_t
        {
            CHAR = 0,
            INTEGER = 1,
            LONG_INTEGER = 2,
            LONG_LONG_INTEGER = 3,
            UNSIGNED_INTEGER = 4,
            UNSIGNED_LONG_INTEGER = 5,
            UNSIGNED_LONG_LONG_INTEGER = 6,
            FLOAT = 7,
            DOUBLE = 8,
        };
        struct LogElement
        {
            LogType type_ = LogType::CHAR;
            union
            {
                /* data */
                char c_;
                int i_;
                long l_;
                long long ll_;
                unsigned u_;
                unsigned long ul_;
                unsigned long long ull_;
                float f_;
                double d_;
            } uU_;
        };
        class Logger final
        {
        public:
            auto flushQueue() noexcept
            {
                while (running_)
                {
                    for (auto next = queue_.getNextToRead(); queue_.size() && next; next = queue_.getNextToRead())
                    {
                        switch (next->type_)
                        {
                        case LogType::CHAR:
                            file_ << next->uU_.c_;
                            break;
                        case LogType::INTEGER:
                            file_ << next->uU_.i_;
                            break;
                        case LogType::LONG_INTEGER:
                            file_ << next->uU_.l_;
                            break;
                        case LogType::LONG_LONG_INTEGER:
                            file_ << next->uU_.ll_;
                            break;
                        case LogType::UNSIGNED_INTEGER:
                            file_ << next->uU_.u_;
                            break;
                        case LogType::UNSIGNED_LONG_INTEGER:
                            file_ << next->uU_.ul_;
                            break;
                        case LogType::UNSIGNED_LONG_LONG_INTEGER:
                            file_ << next->uU_.ull_;
                            break;
                        case LogType::FLOAT:
                            file_ << next->uU_.f_;
                            break;
                        case LogType::DOUBLE:
                            file_ << next->uU_.d_;
                            break;
                        default:
                            FATAL("Unknown log type");
                        }
                        queue_.updateReadIndex();
                    }
                    file_.flush();

                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(10ms);
                }
            }

            explicit Logger(const std::string &file_name) : file_name_(file_name), queue_(LOG_QUEUE_SIZE)
            {
                file_.open(file_name);
                ASSERT(file_.is_open(), "Failed to open log file: " + file_name);
                logger_thread_ = createAndStartThread(-1, "Common/Logger" + file_name,
                                                      [this]()
                                                      { flushQueue(); });
                ASSERT(logger_thread_, "Failed to create logger thread");
            }

            ~Logger()
            {
                std::string time_str;
                std::cerr << getCurrentTimeStr(&time_str) << " Flusing and closing Logger for "
                          << file_name_ << std::endl;
                while (queue_.size())
                {
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(1s);
                }
                running_ = false;
                logger_thread_->join();
                file_.close();
                std::cerr << getCurrentTimeStr(&time_str) << " Closed Logger for "
                          << file_name_ << " exiting " << std::endl;
            }

            auto pushValue(const LogElement &log_element) noexcept
            {
                *(queue_.getNextWriteTo()) = log_element;
                queue_.updateWriteIndex();
            }

            auto pushValue(const char value) noexcept
            {
                pushValue({LogType::CHAR, {value}});
            }
            auto pushValue(const int value) noexcept
            {
                pushValue({LogType::INTEGER, {.i_ = value}});
            }
            auto pushValue(const long value) noexcept
            {
                pushValue({LogType::LONG_INTEGER, {.l_ = value}});
            }
            auto pushValue(const long long value) noexcept
            {
                pushValue({LogType::LONG_LONG_INTEGER, {.ll_ = value}});
            }
            auto pushValue(const unsigned value) noexcept
            {
                pushValue({LogType::UNSIGNED_INTEGER, {.u_ = value}});
            }
            auto pushValue(const unsigned long value) noexcept
            {
                pushValue({LogType::UNSIGNED_LONG_INTEGER, {.ul_ = value}});
            }
            auto pushValue(const unsigned long long value) noexcept
            {
                pushValue({LogType::UNSIGNED_LONG_LONG_INTEGER, {.ull_ = value}});
            }
            auto pushValue(const float value) noexcept
            {
                pushValue({LogType::FLOAT, {.f_ = value}});
            }
            auto pushValue(const double value) noexcept
            {
                pushValue({LogType::DOUBLE, {.d_ = value}});
            }
            auto pushValue(const char *value) noexcept
            {
                while (*value)
                {
                    pushValue(*value);
                    ++value;
                }
            }
            auto pushValue(const std::string &value) noexcept
            {
                pushValue(value.c_str());
            }

            template <typename T, typename... Args>
            auto log(const char *s, const T &value, Args... args) noexcept
            {
                while (*s)
                {
                    if (*s == '%')
                    {
                        if (UNLIKELY(*(s + 1) == '%'))
                        {
                            ++s;
                        }
                        else
                        {
                            pushValue(value);
                            log(s + 1, args...);
                            return;
                        }
                    }

                    pushValue(*s++);
                }
                FATAL("Invalid log format");
            }
            auto log(const char *s) noexcept
            {
                while (*s)
                {
                    if (*s == '%')
                    {
                        if (UNLIKELY(*(s + 1) == '%'))
                        {
                            ++s;
                        }
                        else
                        {
                            FATAL("Invalid log format");
                        }
                    }

                    pushValue(*s++);
                }
            }

            Logger() = delete;
            Logger(const Logger &) = delete;
            Logger(const Logger &&) = delete;
            Logger &operator=(const Logger &) = delete;
            Logger &operator=(const Logger &&) = delete;

        private:
            const std::string file_name_;
            std::ofstream file_;
            LFQueue<LogElement> queue_;
            std::atomic<bool> running_ = {true};
            std::thread *logger_thread_ = nullptr;
        };
    }
}