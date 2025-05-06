#pragma once

#include <sstream>

#include "common/types.hpp"
#include "common/lf_queue.hpp"
using namespace QuantCpp::Common;
namespace QuantCpp::Exchange
{
#pragma pack(push, 1)
    enum class MarketUpdateType : uint8_t
    {
        INVALID = 0,
        ADD = 1,
        MODIFY = 2,
        CANCEL = 3,
        TRADE = 4
    };

    inline std::string marketUpdateTypeToString(MarketUpdateType type)
    {
        switch (type)
        {
        case MarketUpdateType::ADD:
            return "ADD";
        case MarketUpdateType::MODIFY:
            return "MODIFY";
        case MarketUpdateType::CANCEL:
            return "CANCEL";
        case MarketUpdateType::TRADE:
            return "TRADE";
        case MarketUpdateType::INVALID:
            return "INVALID";
        default:
            return "UNKNOWN";
        }
    }
    struct MEMarketUpdate
    {
        MarketUpdateType type_ = MarketUpdateType::INVALID;
        OrderId order_id_ = OrderId_INVALID;
        TickerId ticker_id_ = TickerId_INVALID;
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;
        Qty qty_ = Qty_INVALID;
        Priority priority_ = Priority_INVALID;

        auto toString() const -> std::string
        {
            std::stringstream ss;
            ss << "MarketUpdate [ type:" << marketUpdateTypeToString(type_) << "Ticker: " << tickerIdToString(ticker_id_)
               << " oid: " << orderIdToString(order_id_) << " side: " << sideToString(side_)
               << " price: " << priceToString(price_) << " qty: " << qtyToString(qty_) << " priority: " << priorityToString(priority_) << " ]";
            return ss.str();
        }
    };
#pragma pack(pop)
    // LFQueue가 QuantCpp::Common 네임스페이스에 정의되어 있다면
    using MEMarketUpdateLFQueue = LFQueue<MEMarketUpdate>;

} // namespace QuantCpp::Exchange