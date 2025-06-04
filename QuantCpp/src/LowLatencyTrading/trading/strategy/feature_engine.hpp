#pragma once

#include "common/macros.hpp"
#include "common/logging.hpp"

using namespace QuantCpp::Common;
namespace QuantCpp::Trading
{
    constexpr auto Feature_INVALID = std::numeric_limits<double>::quiet_NaN();
    class FeatureEngine
    {
    public:
        FeatureEngine(Logger *logger) : logger_(logger) {}

        FeatureEngine() = delete;
        // Disable copy and move semantics
        FeatureEngine(const FeatureEngine &) = delete;
        FeatureEngine(const FeatureEngine &&) = delete;
        FeatureEngine &operator=(const FeatureEngine &) = delete;
        FeatureEngine &operator=(const FeatureEngine &&) = delete;

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
             const auto bbo = book->getBBO();
            if (LIKELY(bbo->bid_price_ != Price_INVALID &&
                       bbo->ask_price_ != Price_INVALID))
            {
              mkt_price_ = (bbo->bid_price_ * bbo->ask_qty_ + bbo->ask_price_ * bbo->bid_qty_) /
                           static_cast<double>(bbo->bid_qty_ + bbo->ask_qty_);
            }
            logger_->log("%:% %() % ticker:% price:% side:% mkt-price:% agg-trade-ratio:%\n",
                         __FILE__, __LINE__, __FUNCTION__,
                         getCurrentTimeStr(&time_str_),ticker_id, priceToString(price).c_str(),
                         sideToString(side).c_str(), mkt_price_, agg_trade_qty_ratio_);
                         
        }

        auto onTradeUpdate(const Exchange::MEMarketUpdate *market_update,
                           MarketOrderBook *book) noexcept -> void
        {
            const auto bbo = book->getBBO();
            if (LIKELY(bbo->bid_price_ != Price_INVALID &&
                       bbo->ask_price_ != Price_INVALID))
            {
                agg_trade_qty_ratio_ = static_cast<double>(market_update->qty_) /
                                       (market_update->side_ == Side::BUY ? bbo->ask_qty_ : bbo->bid_qty_);
            }
            logger_->log("%:% %() % % mkt-price:% agg-trade-ratio:%\n",
                         __FILE__, __LINE__, __FUNCTION__,
                         getCurrentTimeStr(&time_str_),
                         market_update->toString().c_str(), mkt_price_, agg_trade_qty_ratio_);
        }

    private:
        std::string time_str_;
        Logger *logger_ = nullptr;

        double mkt_price_ = Feature_INVALID;
        double agg_trade_qty_ratio_ = Feature_INVALID;
    };
} // namespace QuantCpp::Trading