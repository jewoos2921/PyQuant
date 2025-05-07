#pragma once

#include "common/thread_utils.hpp"
#include "common/lf_queue.hpp"
#include "common/macros.hpp"
#include "order_server/client_request.hpp"
#include "order_server/client_response.hpp"
#include "market_data/market_update.hpp"

#include "me_order_book.hpp"

namespace QuantCpp::Exchange
{
    class MatchingEngine final
    {
    public:
        MatchingEngine(ClientRequestLFQueue *client_requests,
                       ClientResponseLFQueue *client_responses,
                       MEMarketUpdateLFQueue *market_updates);
        ~MatchingEngine();
        auto start() -> void;
        auto stop() -> void;
        auto processClientRequest(const MEClientRequest *client_request) noexcept;

        auto sendClientResponse(const MEClientResponse *client_response) noexcept;
        auto sendMarketUpdate(const MEMarketUpdate *market_update) noexcept;
        auto run() noexcept;

    private:
        OrderBookHashMap ticker_order_book_;
    };
} // namespace QuantCpp::Exchange
