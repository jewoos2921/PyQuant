#pragma once

#include "tcp_socket.hpp"

namespace QuantCpp
{
    namespace Common
    {
        struct TCPServer
        {
            explicit TCPServer(Logger &logger) : listener_socket_(logger),
                                                 logger_(logger)
            {
            }

            auto listen(const std::string &iface, int port) -> void;
            auto poll() noexcept -> void;
            auto sendAndRecv() noexcept -> void;

        private:
            auto addToEpollList(TCPSocket *socket);

        public:
            int epoll_fd_ = -1;
            TCPSocket listener_socket_;
            epoll_event events_[1024];

            std::vector<TCPSocket *> receive_sockets_;
            std::vector<TCPSocket *> send_sockets_;

            std::function<void(TCPSocket *, Nanos rx_time)> recv_callback_ = nullptr;
            std::function<void()> recv_finished_callback_ = nullptr;

            std::string time_str_;
            Logger &logger_;
        };

    } // namespace Common

} // namespace QuantCpp
