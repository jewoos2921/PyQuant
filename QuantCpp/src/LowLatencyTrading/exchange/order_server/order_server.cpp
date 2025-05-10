//
// Created by jewoo on 2025-05-09.
//

#include "order_server.hpp"

namespace QuantCpp::Exchange
{

    OrderServer::OrderServer(ClientRequestLFQueue *client_requests, ClientResponseLFQueue *client_responses,
                             const std::string &iface, int port)
        : iface_(iface),
          outgoing_responses_(client_responses),
          logger_("exchange_order_server.log"),
          tcp_server_(logger_),
          fifo_sequencer_(client_requests, &logger_)
    {
        cid_next_outgoing_seq_num_.fill(1);
        cid_next_exp_seq_num_.fill(1);
        cid_tcp_sockets_.fill(nullptr);
        tcp_server_.recv_callback_ = [this](auto socket, auto rx_time)
        {
            recvCallback(socket, rx_time);
        };
        tcp_server_.recv_finished_callback_ = [this]()
        {
            recvFinishedCallback();
        };
    }

    OrderServer::~OrderServer()
    {
        stop();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    auto OrderServer::start() -> void
    {
        run_ = true;
        tcp_server_.listen(iface_, port_);

        ASSERT(createAndStartThread(-1, "Exchange/OrderServer",
                                    [this]()
                                    { run(); }) != nullptr,
               "Failed to start OrderServer thread.");
    }

    auto OrderServer::stop() -> void
    {
        run_ = false;
    }

    auto OrderServer::run() noexcept
    {
        logger_.log("%:% %() %\n",
                    __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_));
        while (run_)
        {
            tcp_server_.poll();
            tcp_server_.sendAndRecv();

            for (auto client_response = outgoing_responses_->getNextToRead();
                 outgoing_responses_->size() && client_response;
                 client_response = outgoing_responses_->getNextToRead())
            {
                auto &next_outgoing_seq_num = cid_next_outgoing_seq_num_[client_response->client_id_];
                logger_.log("%:% %() % Processing cid:% seq:% %\n",
                            __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_),
                            client_response->client_id_, next_outgoing_seq_num, client_response->toString());

                ASSERT(cid_tcp_sockets_[client_response->client_id_] != nullptr,
                       "Don't have a TCPSocket for ClientId:" + std::to_string(client_response->client_id_));
                cid_tcp_sockets_[client_response->client_id_]->send(&next_outgoing_seq_num, sizeof(next_outgoing_seq_num));
                cid_tcp_sockets_[client_response->client_id_]->send(client_response, sizeof(MEClientResponse));

                outgoing_responses_->updateReadIndex();

                ++next_outgoing_seq_num;
            }
        }
    }
    auto OrderServer::recvCallback(TCPSocket *socket, Nanos rx_time) noexcept
    {
        logger_.log("%:% %() % Received socket:% len:% rx:%\n", __FILE__, __LINE__, __FUNCTION__,
                    getCurrentTimeStr(&time_str_), socket->socket_fd_,
                    socket->next_rcv_valid_index_, rx_time);

        if (socket->next_rcv_valid_index_ >= sizeof(OMClientRequest))
        {
            size_t i = 0;
            for (; i + sizeof(OMClientRequest) <= socket->next_rcv_valid_index_; i += sizeof(OMClientRequest))
            {
                auto request = reinterpret_cast<const OMClientRequest *>(socket->inbound_data_.data() + i);
                logger_.log("%:% %() % Received %\n",
                            __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_),
                            request->toString());
                if (UNLIKELY(cid_tcp_sockets_[request->me_client_request_.client_id_] == nullptr))
                {
                    cid_tcp_sockets_[request->me_client_request_.client_id_] = socket;
                }

                if (cid_tcp_sockets_[request->me_client_request_.client_id_] != socket)
                {
                    logger_.log("%:% %() % Received ClientRequest from ClientId:% on different socket:% expected:%\n",
                                __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_),
                                request->me_client_request_.client_id_, socket->socket_fd_,
                                cid_tcp_sockets_[request->me_client_request_.client_id_]->socket_fd_);
                    continue;
                }

                auto &next_exp_seq_num = cid_next_exp_seq_num_[request->me_client_request_.client_id_];
                if (request->seq_num_ != next_exp_seq_num)
                {
                    logger_.log("%:% %() % Incorrect sequence number. ClientId:% SeqNum expected:% received:%\n",
                                __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_),
                                request->me_client_request_.client_id_,
                                next_exp_seq_num, request->seq_num_);
                    continue;
                }

                ++next_exp_seq_num;

                fifo_sequencer_.addClientRequest(rx_time, request->me_client_request_);
            }

            memcpy(socket->inbound_data_.data(), socket->inbound_data_.data() + i,
                   socket->next_rcv_valid_index_ - i);
            socket->next_rcv_valid_index_ -= i;
        }
    }
    auto OrderServer::recvFinishedCallback() noexcept
    {
        fifo_sequencer_.sequenceAndPublish();
    }

}