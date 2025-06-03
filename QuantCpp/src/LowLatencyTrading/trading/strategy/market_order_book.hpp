#pragma once

#include "common/types.hpp"
#include "common/mem_pool.hpp"
#include "common/logging.hpp"

#include "market_order.hpp"
#include "exchange/market_data/market_update.hpp"

namespace QuantCpp::Trading
{
    class TradeEngine;
    class MarketOrderBook final
    {
    public:
        MarketOrderBook() = delete;
        // Disable copy and move semantics
        MarketOrderBook(const MarketOrderBook &) = delete;
        MarketOrderBook(MarketOrderBook &&) = delete;
        MarketOrderBook &operator=(const MarketOrderBook &) = delete;
        MarketOrderBook &operator=(MarketOrderBook &&) = delete;

    private:
        const TickerId ticker_id_;
        TradeEngine *trade_engine_ = nullptr;
        OrderHashMap oid_to_order_;

        Logger *logger_ = nullptr;

        OrdersAtPriceHashMap orders_at_price_map_;
    };
} // namespace QuantCpp::Trading