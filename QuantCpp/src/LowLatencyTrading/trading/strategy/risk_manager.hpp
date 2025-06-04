#pragma once

#include "common/macros.hpp"
#include "common/logging.hpp"
#include "position_keeper.hpp"
#include "om_order.hpp"

namespace QuantCpp::Trading
{
    class OrderManager;
    enum class RiskCheckResult : int8_t
    {
        INVALID = 0,
        ORDER_TOO_LARGE = 1,
        POSITION_TOO_LARGE = 2,
        LOSS_TOO_LARGE = 3,
        ALLOWED = 4
    };

    inline auto riskCheckResultToString(RiskCheckResult result) -> std::string
    {
        switch (result)
        {
        case RiskCheckResult::INVALID:
            return "INVALID";
        case RiskCheckResult::ORDER_TOO_LARGE:
            return "ORDER_TOO_LARGE";
        case RiskCheckResult::POSITION_TOO_LARGE:
            return "POSITION_TOO_LARGE";
        case RiskCheckResult::LOSS_TOO_LARGE:
            return "LOSS_TOO_LARGE";
        case RiskCheckResult::ALLOWED:
            return "ALLOWED";
        default:
            return "";
        }
    }
    class RiskManager
    {
    public:
        RiskManager(Logger *logger, PositionKeeper *position_keeper)
            : logger_(logger), position_keeper_(position_keeper);

        RiskManager() = delete;
        // Disable copy and move semantics
        RiskManager(const RiskManager &) = delete;
        RiskManager(const RiskManager &&) = delete;
        RiskManager &operator=(const RiskManager &) = delete;
        RiskManager &operator=(const RiskManager &&) = delete;

        auto onOMOrderUpdate(const OMOrder &order) noexcept -> void;

    private:
        Logger *logger_;
    };
} // namespace QuantCpp::Trading