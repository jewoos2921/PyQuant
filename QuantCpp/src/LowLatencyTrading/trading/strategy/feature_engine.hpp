#pragma once

#include "common/macros.hpp"
#include "common/logging.hpp"

using namespace QuantCpp::Common;
namespace QuantCpp::Trading
{
    constexpr auto FeatureINVALID = std::numeric_limits<double>::quiet_NaN();
    class FeatureEngine
    {
    public:
        FeatureEngine(Logger *logger) : logger_(logger) {}
        FeatureEngine() = delete;
        // Disable copy and move semantics
        FeatureEngine(const FeatureEngine &) = delete;
        FeatureEngine(FeatureEngine &&) = delete;
        FeatureEngine &operator=(const FeatureEngine &) = delete;
        FeatureEngine &operator=(FeatureEngine &&) = delete;

        auto getMktPrice() const noexcept -> double
        {
            return mkt_price_;
        }

        auto getAggTradeQtyRatio() const noexcept -> double
        {
            return agg_trade_qty_ratio_;
        }

        auto onOrderBookUpdate(TickerId ticker_id, Price price,
                               Side side, MarketOrderBook *book) noexcept -> void
        {
        }

    private:
        std::string time_str_;
        Logger *logger_ = nullptr;

        double mkt_price_ = FeatureINVALID;
        double agg_trade_qty_ratio_ = FeatureINVALID;
    };
} // namespace QuantCpp::Trading