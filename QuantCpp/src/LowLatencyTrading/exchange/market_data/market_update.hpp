#pragma once

#include <sstream>

#include "common/types.hpp"
#include "common/lf_queue.hpp"

using namespace QuantCpp::Common;
namespace QuantCpp::Exchange
{

    enum class MarketUpdateType : uint8_t
    {
        INVALID = 0,
        CLEAR = 1,
        ADD = 2,
        MODIFY = 3,
        CANCEL = 4,
        TRADE = 5,
        SNAPSHOT_START = 6,
        SNAPSHOT_END = 7
    };

    inline std::string marketUpdateTypeToString(MarketUpdateType type)
    {
        switch (type)
        {
        case MarketUpdateType::CLEAR:
            return "CLEAR";
        case MarketUpdateType::ADD:
            return "ADD";
        case MarketUpdateType::MODIFY:
            return "MODIFY";
        case MarketUpdateType::CANCEL:
            return "CANCEL";
        case MarketUpdateType::TRADE:
            return "TRADE";
        case MarketUpdateType::SNAPSHOT_START:
            return "SNAPSHOT_START";
        case MarketUpdateType::SNAPSHOT_END:
            return "SNAPSHOT_END";
        case MarketUpdateType::INVALID:
            return "INVALID";
        default:
            return "UNKNOWN";
        }
    }

#pragma pack(push, 1)
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

    struct MDPMarketUpdate
    {
        size_t seq_num_{0};
        MEMarketUpdate me_market_update_;

        auto toString() const -> std::string
        {
            std::stringstream ss;
            ss << "MDPMarketUpdate [ seq_num: " << seq_num_ << " " << me_market_update_.toString() << " ]";
            return ss.str();
        }
    };
#pragma pack(pop)
    // LFQueue가 QuantCpp::Common 네임스페이스에 정의되어 있다면
    using MEMarketUpdateLFQueue = LFQueue<MEMarketUpdate>;
    using MDPMarketUpdateLFQueue = LFQueue<MDPMarketUpdate>;

} // namespace QuantCpp::Exchange