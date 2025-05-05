#pragma once
#include <iostream>
#include <atomic>
#include <thread>
#include <unistd.h>
#include <sys/syscall.h>

namespace QuantCpp
{
    namespace Common
    {
        inline auto setThreadCore(int core_id) noexcept
        {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(core_id, &cpuset);
            return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0;
        }

        template <typename T, typename... Args>
        inline auto createAndStartThread(int core_id,
                                         const std::string &name, T &&func, Args &&...args) noexcept
        {
            auto t = new std::thread([&]()
                                     {
                if (core_id >= 0 && !setThreadCore(core_id))
                {
                    std::cerr << "Failed to set thread core affinity for " << name << " " << pthread_self() << " to " << std::endl;
                    exit(EXIT_FAILURE);
                }
                std::cerr << "Set core affinity for " << name << " " << pthread_self() << " to " << std::endl; 
                std::forward<T>(func)(std::forward<Args>(args)...); });

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
            return t;
        }
    }
} // namespace QuantCpp