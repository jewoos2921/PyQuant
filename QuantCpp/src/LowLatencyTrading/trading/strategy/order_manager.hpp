#pragma once
#include "common/macros.hpp"
#include "common/logging.hpp"

#include "exchange/order_server/client_response.hpp"

#include "om_order.hpp"
#include "risk_manager.hpp"

namespace QuantCpp::Trading
{
    class TradeEngine;
    class OrderManager
    {
    public:
        OrderManager(Logger *logger, TradeEngine *trade_engine,
                     RiskManager &risk_manager)
            : trade_engine_(trade_engine), risk_manager_(risk_manager), logger_(logger)
        {
        }

        ~OrderManager() = default;

        OrderManager(const OrderManager &) = delete;
        OrderManager(const OrderManager &&) = delete;
        OrderManager &operator=(const OrderManager &) = delete;
        OrderManager &operator=(const OrderManager &&) = delete;

        auto onOrderUpdate(const Exchange::MEClientResponse *client_response) noexcept -> void;
        auto newOrder(OMOrder *order,
                      TickerId ticker_id, Price price, Side side, Qty qty) noexcept -> void;
        auto cancelOrder(OMOrder *order) noexcept -> void;
        auto moveOrder(OMOrder *order, TickerId ticker_id, Price price, Side side, Qty qty) noexcept -> void;
        auto moveOrders(TickerId ticker_id,
                        Price bid_price, Price ask_price, Qty clip) noexcept;
        auto getOMOrderSideHashMap(TickerId ticker_id) const;

    private:
        TradeEngine *trade_engine_ = nullptr;
        const RiskManager &risk_manager_;

        std::string time_str_;
        Logger *logger_ = nullptr;

        OMOrderTickerSideHashMap ticker_side_orders_;
        OrderId next_order_id_ = 1;
    };
}