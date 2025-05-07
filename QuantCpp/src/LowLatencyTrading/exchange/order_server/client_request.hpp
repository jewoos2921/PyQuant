#pragma once

#include <sstream>

#include "common/types.hpp"
#include "common/lf_queue.hpp"

using namespace QuantCpp::Common;
namespace QuantCpp::Exchange
{
#pragma pack(push, 1)
    enum class ClientRequestType : uint8_t
    {
        INVALID = 0,
        NEW = 1,
        CANCEL = 2
    };

    inline std::string clientReqeustTypeToString(ClientRequestType type)
    {
        switch (type)
        {
        case ClientRequestType::INVALID:
            return "INVALID";
        case ClientRequestType::NEW:
            return "NEW";
        case ClientRequestType::CANCEL:
            return "CANCEL";
        default:
            return "UNKNOWN";
        }
    }

    struct MEClientRequest
    {
        ClientRequestType type_ = ClientRequestType::INVALID;

        ClientId client_id_ = ClientId_INVALID;
        TickerId ticker_id_ = TickerId_INVALID;

        OrderId client_order_id_ = OrderId_INVALID;
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;
        Qty qty_ = Qty_INVALID;

        auto toString() const
        {
            std::stringstream ss;
            ss << "MEClientRequest [ " << "type: " << clientReqeustTypeToString(type_) << " client:"
               << clientIdToString(client_id_)
               << " ticker:" << tickerIdToString(ticker_id_)
               << " client_order_id:" << orderIdToString(client_order_id_)
               << " side:" << sideToString(side_)
               << " qty:" << qtyToString(qty_)
               << " price:" << priceToString(price_)
               << " ]";
            return ss.str();
        }
    };
#pragma pack(pop)

    using ClientRequestLFQueue = LFQueue<MEClientRequest>;

}
