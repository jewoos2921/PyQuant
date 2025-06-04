#pragma once

#include <array>
#include <sstream>
#include "common/types.hpp"

using namespace QuantCpp::Common;

namespace QuantCpp::Trading
{
    enum class OMOrderState : int8_t
    {
        INVALID = 0,
        PENDING_NEW = 1,
        LIVE = 2,
        PENDING_CANCEL = 3,
        DEAD = 4
    };

    inline auto OMOrderStateToString(OMOrderState state) -> std::string
    {
        switch (state)
        {
        case OMOrderState::INVALID:
            return "INVALID";
        case OMOrderState::PENDING_NEW:
            return "PENDING_NEW";
        case OMOrderState::LIVE:
            return "LIVE";
        case OMOrderState::PENDING_CANCEL:
            return "PENDING_CANCEL";
        case OMOrderState::DEAD:
            return "DEAD";
        default:
            return "UNKNOWN";
        }
    }

    struct OMOrder
    {
        TickerId ticker_id_ = TickerId_INVALID;
        OrderId order_id_ = OrderId_INVALID;
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;
        Qty qty_ = Qty_INVALID;
        OMOrderState order_state_ = OMOrderState::INVALID;

        auto to_string() const -> std::string
        {
            std::ostringstream oss;
            oss << "OMOrder{"
                << "ticker_id: " << tickerIdToString(ticker_id_)
                << ", order_id: " << orderIdToString(order_id_)
                << ", side: " << sideToString(side_)
                << ", price: " << priceToString(price_)
                << ", qty: " << qtyToString(qty_)
                << ", state: " << OMOrderStateToString(order_state_)
                << "}";
            return oss.str();
        }
    };

    using OMOrderSideHashMap = std::array<OMOrder, sideToIndex(Side::MAX) + 1>;
    using OMOrderTickerSideHashMap = std::array<OMOrderSideHashMap, ME_MAX_TICKERS>;

} // namespace QuantCpp::Trading
