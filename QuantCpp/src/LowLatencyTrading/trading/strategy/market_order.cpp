#include "market_order.hpp"
namespace QuantCpp::Trading
{
    auto MarketOrder::toString() const -> std::string
    {
        std::stringstream ss;
        ss << "MarketOrder{"
           << "order_id: " << orderIdToString(order_id_)
           << ", side: " << sideToString(side_)
           << ", price: " << priceToString(price_)
           << ", qty: " << qtyToString(qty_)
           << ", priority: " << priorityToString(priority_)
           << ", prev_order: " << orderIdToString(prev_order_ ? prev_order_->order_id_ : OrderId_INVALID)
           << ", next_order: " << orderIdToString(next_order_ ? next_order_->order_id_ : OrderId_INVALID)
           << '}';
        return ss.str();
    }
}