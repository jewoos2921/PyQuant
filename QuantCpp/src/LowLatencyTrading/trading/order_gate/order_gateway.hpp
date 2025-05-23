#pragma once
#include <functional>

#include "common/thread_utils.hpp"
#include "common/macros.hpp"
#include "common/tcp_server.hpp"

#include "exchange/order_server/client_request.hpp"
#include "exchange/order_server/client_response.hpp"

namespace QuantCpp::Trading
{
    class OrderGateway
    {
    public:
        OrderGateway(
            ClientId client_id,
            Exchange::ClientRequestLFQueue *client_requests,
            Exchange::ClientResponseLFQueue *client_responses,
            std::string ip,
            const std::string &iface,
            int port);

        ~OrderGateway();

        auto start() -> void;

        auto stop() -> void;

        OrderGateway() = delete;
        OrderGateway(const OrderGateway &) = delete;
        OrderGateway(const OrderGateway &&) = delete;
        OrderGateway &operator=(const OrderGateway &) = delete;
        OrderGateway &operator=(const OrderGateway &&) = delete;

    private:
        const ClientId client_id_;
        Exchange::ClientRequestLFQueue *outgoing_requests_ = nullptr;
        Exchange::ClientResponseLFQueue *incoming_responses_ = nullptr;

        volatile bool run_{false};

        std::string time_str_;
        Logger logger_;
        std::string ip_;
        bool is_recovery_{false};
        const std::string iface_;
        const int port_;

        size_t next_outgoing_seq_num_{1};
        size_t next_exp_seq_num_{1};
        TCPSocket tcp_socket_;

    private:
        auto run() noexcept -> void;
        auto recvCallback(TCPSocket *socket,
                          Nanos rx_time) noexcept -> void;
    };
}