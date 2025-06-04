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
        MarketOrderBook(TickerId ticker_id, Logger *logger);
        ~MarketOrderBook();
        auto onMarketUpdate(const Exchange::MEMarketUpdate *market_update) noexcept -> void;
        auto setTradeEngine(TradeEngine *trade_engine);
        auto updateBBO(bool update_bid, bool update_ask) noexcept;
        auto getBBO() const noexcept -> const BBO *;
        auto toString(bool detailed, bool validity_check) const -> std::string;

        MarketOrderBook() = delete;
        // Disable copy and move semantics
        MarketOrderBook(const MarketOrderBook &) = delete;
        MarketOrderBook(const MarketOrderBook &&) = delete;
        MarketOrderBook &operator=(const MarketOrderBook &) = delete;
        MarketOrderBook &operator=(const MarketOrderBook &&) = delete;

    private:
        const TickerId ticker_id_;
        TradeEngine *trade_engine_ = nullptr;
        OrderHashMap oid_to_order_;

        MemPool<MarketOrdersAtPrice> orders_at_price_pool_;
        MarketOrdersAtPrice *bids_by_price_ = nullptr;
        MarketOrdersAtPrice *asks_by_price_ = nullptr;

        OrdersAtPriceHashMap price_orders_at_price_;

        MemPool<MarketOrder> order_pool_;
        BBO bbo_;

        Logger *logger_ = nullptr;
        std::string time_str_;
        OrdersAtPriceHashMap orders_at_price_map_;

    private:
        auto priceToIndex(Price price) const noexcept;
        auto getOrdersAtPrice(Price price) noexcept -> MarketOrdersAtPrice *;
        auto addOrdersAtPrice(MarketOrdersAtPrice *new_orders_at_price) noexcept;
        auto removeOrdersAtPrice(Side side, Price price) noexcept  ;
        auto removeOrder(MarketOrder *order) noexcept -> void;
        auto addOrder(MarketOrder *order) noexcept -> void;
    };

    using MarketOrderBookHashMap = std::array<MarketOrderBook *, ME_MAX_TICKERS>; // Example size, adjust as needed
} // namespace QuantCpp::Trading