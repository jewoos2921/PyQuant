#pragma once

#include <array>
#include <sstream>
#include "common/types.hpp"

using namespace QuantCpp::Common;

namespace QuantCpp::Trading
{
    struct MarketOrder
    {
        OrderId order_id_ = OrderId_INVALID;
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;
        Qty qty_ = Qty_INVALID;
        Priority priority_ = Priority_INVALID;

        MarketOrder *prev_order_ = nullptr; // Pointer to the previous order in the linked list
        MarketOrder *next_order_ = nullptr; // Pointer to the next order in the linked list

        MarketOrder() = default;
        MarketOrder(OrderId order_id, Side side, Price price, Qty qty, Priority priority)
            : order_id_(order_id), side_(side), price_(price), qty_(qty), priority_(priority) {}
        auto toString() const -> std::string;
    };

    using OrderHashMap = std::array<MarketOrder *, ME_MAX_ORDER_IDS>; // Example size, adjust as needed

    struct MarketOrdersAtPrice
    {
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;

        MarketOrder *first_mkt_order_ = nullptr; // Pointer to the first order in the linked list

        MarketOrdersAtPrice *prev_entry_ = nullptr; // Pointer to the previous entry in the linked list
        MarketOrdersAtPrice *next_entry_ = nullptr; // Pointer to the next entry in the linked list
        MarketOrdersAtPrice() = default;
        MarketOrdersAtPrice(Side side, Price price)
            : side_(side), price_(price), first_mkt_order_(nullptr),
              prev_entry_(nullptr), next_entry_(nullptr) {}
        auto toString() const -> std::string
        {
            std::stringstream ss;
            ss << "MarketOrdersAtPrice{"
               << "side: " << sideToString(side_)
               << ", price: " << priceToString(price_)
               << ", first_mkt_order: " << (first_mkt_order_ ? first_mkt_order_->toString() : "nullptr")
               << ", prev_entry: " << priceToString(prev_entry_ ? prev_entry_->price_ : Price_INVALID) << " "
               << ", next_entry: " << priceToString(next_entry_ ? next_entry_->price_ : Price_INVALID)
               << '}';
            return ss.str();
        }
    };
    using OrdersAtPriceHashMap = std::array<MarketOrdersAtPrice *, ME_MAX_PRICE_LEVELS>; // Example size, adjust as needed

    struct BBO
    {
        Price bid_price_ = Price_INVALID;
        Price ask_price_ = Price_INVALID;
        Qty bid_qty_ = Qty_INVALID;
        Qty ask_qty_ = Qty_INVALID;
        auto toString() const -> std::string
        {
            std::stringstream ss;
            ss << "BBO{"
               << qtyToString(bid_qty_) << "@" << priceToString(bid_price_)
               << "X"
               << priceToString(ask_price_) << "@" << qtyToString(ask_qty_)
               << "}";
            return ss.str();
        }
    };

} // namespace QuantCpp::Trading
