#pragma once

#include <array>
#include <sstream>
#include "common/types.hpp"

using namespace QuantCpp::Common;

namespace QuantCpp::Trading
{
    enum class OMOrderState : int8_t
    {
        INVALID = 0,
        PENDING_NEW = 1,
        LIVE = 2,
        PENDING_CALL = 3,
        DEAD = 4
    };

    
} // namespace QuantCpp::Trading
