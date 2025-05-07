#pragma once

#include "common/types.hpp"
#include "common/mem_pool.hpp"
#include "common/logging.hpp"
#include "order_server/client_response.hpp"
#include "market_data/market_update.hpp"

#include "me_order.hpp"

using namespace QuantCpp::Common;

namespace QuantCpp::Exchange
{
    class MatchingEngine;
    class MEOrderBook final
    {
    public:
        explicit MEOrderBook(TickerId ticker_id,
                             Logger *logger, MatchingEngine *matching_engine);

        ~MEOrderBook();

        auto add(ClientId client_id, OrderId client_order_id,
                 TickerId ticker_id, Side side, Price price, Qty qty) noexcept -> void;
        auto cancel(ClientId client_id, OrderId order_id,
                    TickerId ticker_id) noexcept -> void;
        auto toString(bool detailed, bool validity_check) const -> std::string;

        MEOrderBook() = delete;
        MEOrderBook(const MEOrderBook &) = delete;
        MEOrderBook(const MEOrderBook &&) = delete;
        MEOrderBook &operator=(const MEOrderBook &) = delete;
        MEOrderBook &operator=(const MEOrderBook &&) = delete;

    private:
        TickerId ticker_id_ = TickerId_INVALID;
        MatchingEngine *matching_engine_ = nullptr;
        ClientOrdersHashMap cid_oid_to_order_;

        MemPool<MEOrdersAtPrice> orders_at_price_pool_;
        MEOrdersAtPrice *bids_by_price_ = nullptr;
        MEOrdersAtPrice *asks_by_price_ = nullptr;

        OrdersAtPriceHashMap price_orders_at_price_;

        MemPool<MEOrder> order_pool_;

        MEClientResponse client_response_;
        MEMarketUpdate market_update_;

        OrderId next_market_order_id_ = 1;
        std::string time_str_;
        Logger *logger_ = nullptr;

    private:
        auto generateNewMarketOrderId() noexcept -> OrderId
        {
            return next_market_order_id_++;
        }
        auto priceToIndex(Price price) const noexcept
        {
            return (price % ME_MAX_PRICE_LEVELS);
        }
        auto getOrdersAtPrice(Price price) noexcept -> MEOrdersAtPrice *
        {
            auto index = priceToIndex(price);
            return price_orders_at_price_.at(index);
        }
        auto addOrdersAtPrice(MEOrdersAtPrice *new_orders_at_price) noexcept;
        auto removeOrdersAtPrice(Side side, Price price) noexcept;
        auto getNextPriority(Price price) noexcept;

        auto match(TickerId ticker_id,
                   ClientId client_id, Side side,
                   OrderId client_order_id,
                   OrderId new_market_order_id,
                   MEOrder *bid_itr, Qty *leaves_qty) noexcept;

        auto checkForMatch(ClientId client_id,
                           OrderId client_order_id,
                           TickerId ticker_id,
                           Side side,
                           Price price,
                           Qty qty,
                           Qty new_market_order_id) noexcept;
                           
        auto removeOrder(MEOrder *order) noexcept -> void;
        auto addOrder(MEOrder *order) noexcept;
    };
    using OrderBookHashMap = std::array<MEOrderBook *, ME_MAX_TICKERS>;
}