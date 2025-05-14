#pragma once
#include <functional>

#include "market_data/snapshot_synthesizer.hpp"

namespace QuantCpp::Exchange
{
    class MarketDataPublisher
    {
    public:
    private:
        size_t next_inc_seq_num_{1};
    };

} // namespace QuantCpp::Exchange
