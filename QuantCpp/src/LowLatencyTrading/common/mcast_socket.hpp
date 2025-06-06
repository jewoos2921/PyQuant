#pragma once
#include <functional>

#include "socket_utils.hpp"
#include "logging.hpp"

namespace QuantCpp
{
    namespace Common
    {
        constexpr size_t McastBufferSize = 64 * 1024 * 1024;
        struct McastSocket
        {
            McastSocket(Logger &logger) : logger_(logger)
            {
                outbound_data_.resize(McastBufferSize);
                inbound_data_.resize(McastBufferSize);
            }

            auto init(const std::string &ip,
                      const std::string &iface, int port, bool is_listening) -> int;
            auto join(const std::string &ip) -> bool;
            auto leave(const std::string &ip, int port) -> void;
            auto sendAndRecv() noexcept -> bool;
            auto send(const void *data, size_t len) -> bool;
            int socket_fd_ = -1;
            std::vector<char> outbound_data_;
            size_t next_send_valid_index_ = 0;
            std::vector<char> inbound_data_;
            size_t next_recv_valid_index_ = 0;
            Logger &logger_;

            std::function<void(McastSocket *s)> recv_callback_ = nullptr;
            std::string time_str_;
        };

    } // namespace Common

} // namespace QuantCpp
