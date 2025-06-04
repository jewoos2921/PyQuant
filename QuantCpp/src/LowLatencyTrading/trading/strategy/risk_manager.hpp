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
    struct RiskInfo
    {
        const PositionInfo *position_info_ = nullptr;
        RiskCfg rick_cfg_;

        auto checkPreTradeRisk(Side side, Qty qty) const noexcept
        {
            if (UNLIKELY(qty > rick_cfg_.max_order_size_))
            {
                return RiskCheckResult::ORDER_TOO_LARGE;
            }
            if (UNLIKELY(std::abs(position_info_->position_ + sideToValue(side) *
                                                                  static_cast<int32_t>(qty)) > static_cast<int32_t>(rick_cfg_.max_position_)))
            {
                return RiskCheckResult::POSITION_TOO_LARGE;
            }
            if (UNLIKELY(position_info_->total_pnl_ < rick_cfg_.max_loss_))
            {
                return RiskCheckResult::LOSS_TOO_LARGE;
            }
            return RiskCheckResult::ALLOWED;
        }

        auto toString() const -> std::string
        {
            std::stringstream ss;
            ss << "RiskInfo["
               << "position_info:" << position_info_->toString()
               << ", risk_cfg:" << rick_cfg_.toString()
               << "]";
            return ss.str();
        }
    };

    using TickerRiskInfoHashMap = std::array<RiskInfo, ME_MAX_TICKERS>;

    class RiskManager
    {
    public:
        RiskManager(Logger *logger, const PositionKeeper *position_keeper,
                    const TradeEngineCfgHashMap &ticker_cfg);

        auto checkPreTradeRisk(TickerId ticker_id,
                               Side side, Qty qty) const noexcept;

        RiskManager() = delete;
        // Disable copy and move semantics
        RiskManager(const RiskManager &) = delete;
        RiskManager(const RiskManager &&) = delete;
        RiskManager &operator=(const RiskManager &) = delete;
        RiskManager &operator=(const RiskManager &&) = delete;

    private:
        Logger *logger_;
        std::string time_str_;

        TickerRiskInfoHashMap ticker_risk_;
    };
} // namespace QuantCpp::Trading