#include "market_data_consumer.hpp"

namespace QuantCpp::Trading
{
    MarketDataConsumer::MarketDataConsumer(
        ClientId client_id,
        Exchange::MEMarketUpdateLFQueue *market_updates,
        const std::string &iface,
        const std::string &snapshot_ip, int snapshot_port,
        const std::string &incremental_ip, int incremental_port) : incoming_md_updates_(market_updates),
                                                                   run_(false),
                                                                   logger_("traiding_market_data_consumer_" + std::to_string(client_id) + ".log"),
                                                                   incremental_mcast_socket_(logger_), snapshot_mcast_socket_(logger_),
                                                                   iface_(iface), snapshot_ip_(snapshot_ip),
                                                                   snapshot_port_(snapshot_port),
    {
        auto recv_callback = [this](auto socket)
        {
            recvCallback(socket);
        };

        incremental_mcast_socket_.recv_callback_ = recv_callback;
        ASSERT(incremental_mcast_socket_.init(incremental_ip, iface, incremental_port, true) >= 0,
               "Failed to initialize incremental multicast socket. error: " +
                   std::to_string(errno));
        ASSERT(incremental_mcast_socket_.join(incremental_ip), "join failed on: " + std::to_string(incremental_mcast_socket_.socket_fd_) + " error:" + std::string(std::strerror(errno)));
        snapshot_mcast_socket_.recv_callback_ = recv_callback;
    }

    MarketDataConsumer::~MarketDataConsumer()
    {
        stop();
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(5s);
    }

    auto MarketDataConsumer::start() -> void
    {
        run_ = true;
        ASSERT(createAndStartThread(-1, "Trading/MarketDataConsumer",
                                    [this]()
                                    { run(); }) != nullptr,
               "Failed to start MarketData thread.");
    }
    auto MarketDataConsumer::stop() -> void { run_ = false; }

    auto MarketDataConsumer::run() noexcept -> void
    {
        logger_.log("%:% %() %\n",
                    __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_));
        while (run_)
        {
            incremental_mcast_socket_.sendAndRecv();
            snapshot_mcast_socket_.sendAndRecv();
        }
    }
    auto MarketDataConsumer::recvCallback(McastSocket *socket) noexcept -> void
    {
    }
    auto MarketDataConsumer::queueMessage(bool is_snapshot,
                                          const Exchange::MDPMarketUpdate *request)
    {
    }
    auto MarketDataConsumer::startSnapshotSync() -> void
    {
    }
    auto MarketDataConsumer::checkSnapshotSync() -> void
    {
    }
}