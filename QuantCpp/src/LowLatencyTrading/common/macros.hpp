#pragma once

#include <cstring>
#include <iostream>

namespace QuantCpp
{
    namespace Common
    {
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

        inline auto ASSERT(bool condition, const std::string &message) -> void
        {
            if (UNLIKELY(!condition))
            {
                std::cerr << "Assertion failed: " << message << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        inline auto FATAL(const std::string &message) -> void
        {
            std::cerr << "Fatal error: " << message << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}