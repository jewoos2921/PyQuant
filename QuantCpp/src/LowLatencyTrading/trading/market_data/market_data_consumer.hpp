#pragma once

#include <functional>
#include <map>

#include "common/thread_utils.hpp"
#include "common/lf_queue.hpp"
#include "common/macros.hpp"
#include "common/mcast_socket.hpp"

#include "exchange/market_data/market_update.hpp"

namespace QuantCpp::Trading
{
    class MarketDataConsumer
    {
    public:
    public:
        MarketDataConsumer(
            ClientId client_id,
            Exchange::MEMarketUpdateLFQueue *market_updates,
            const std::string &iface,
            const std::string &snapshot_ip, int snapshot_port,
            const std::string &incremental_ip, int incremental_port);
        ~MarketDataConsumer();

        auto start() -> void;
        auto stop() -> void;
      

        MarketDataConsumer() = delete;
        MarketDataConsumer(const MarketDataConsumer &) = delete;
        MarketDataConsumer(const MarketDataConsumer &&) = delete;
        MarketDataConsumer &operator=(const MarketDataConsumer &) = delete;
        MarketDataConsumer &operator=(const MarketDataConsumer &&) = delete;

    private:
        size_t next_exp_inc_seq_num_{1};
        Exchange::MEMarketUpdateLFQueue *incoming_md_updates_ = nullptr;
        volatile bool run_{false};

        std::string time_str_;
        Logger logger_;
        McastSocket incremental_mcast_socket_, snapshot_mcast_socket_;

        bool in_recovery_{false};
        const std::string iface_, snapshot_ip_;
        const int snapshot_port_;
        using QueueMarketUpdates = std::map<size_t, Exchange::MEMarketUpdate>;
        QueueMarketUpdates snapshot_queued_msgs_, incremental_queued_msgs_;

    private:
        auto run() noexcept -> void;
        auto recvCallback(McastSocket *socket) noexcept -> void;
        auto queueMessage(bool is_snapshot,
                          const Exchange::MDPMarketUpdate *request);
        auto startSnapshotSync() -> void;
        auto checkSnapshotSync() -> void;
    };
}