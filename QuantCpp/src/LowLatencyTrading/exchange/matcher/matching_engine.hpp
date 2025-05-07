#pragma once

#include "common/thread_utils.hpp"
#include "common/lf_queue.hpp"
#include "common/macros.hpp"
#include "old_server/client_request.hpp"
#include "old_server/client_response.hpp"
#include "market_data/market_update.hpp"
#include "me_order.hpp"

namespace QuantCpp::Exchange
{
    class MatchingEngine final
    {
    private:
        OrderBookHashMap ticker_order_book_;
    };
} // namespace QuantCpp::Exchange
