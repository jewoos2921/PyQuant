//
// Created by jewoo on 2025-05-09.
//

#pragma once

#include <functional>
#include "common/thread_utils.hpp"
#include "common/macros.hpp"
#include "common/tcp_server.hpp"
#include "order_server/client_request.hpp"
#include "order_server/client_response.hpp"
#include "order_server/fifo_sequencer.hpp"

namespace QuantCpp::Exchange
{
    class OrderServer
    {
    public:
        OrderServer(ClientRequestLFQueue *client_requests,
                    ClientResponseLFQueue *client_responses,
                    const std::string &iface,
                    int port);

        ~OrderServer();

        auto start() -> void;

        auto stop() -> void;

        auto run() noexcept;

        auto recvCallback(TCPSocket *, Nanos rx_time) noexcept;
        auto recvFinishedCallback() noexcept;

    private:
        const std::string iface_;
        const int port_{0};
        ClientResponseLFQueue *outgoing_responses_{nullptr};

        volatile bool run_{false};
        Logger logger_;
        std::string time_str_;

        std::array<size_t, ME_MAX_NUM_CLIENTS> cid_next_outgoing_seq_num_;
        std::array<size_t, ME_MAX_NUM_CLIENTS> cid_next_exp_seq_num_;

        std::array<TCPSocket *, ME_MAX_NUM_CLIENTS> cid_tcp_sockets_;

        TCPServer tcp_server_;

        FIFOSequencer fifo_sequencer_;
    };
}