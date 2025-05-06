#include "mcast_socket.hpp"

namespace QuantCpp::Common
{
    auto McastSocket::init(const std::string &ip, const std::string &iface, int port, bool is_listening) -> int
    {
        return 0;
    }

    auto McastSocket::join(const std::string &ip) -> bool
    {
        return Common::join(socket_fd_, ip);
    }

    auto McastSocket::leave(const std::string &ip, int port) -> void
    {
        close(socket_fd_);
        socket_fd_ = -1;
    }

    auto McastSocket::sendAndRecv() noexcept -> bool
    {
        const ssize_t n_rcv = recv(socket_fd_, inbound_data_.data() + next_recv_valid_index_,
                                   McastBufferSize - next_recv_valid_index_, MSG_DONTWAIT);
        if (n_rcv > 0)
        {
            next_recv_valid_index_ += n_rcv;
            logger_.log("%:% %() % read socket:% len:%\n",
                        __FILE__, __LINE__, __FUNCTION__,
                        getCurrentTimeStr(&time_str_), socket_fd_, next_recv_valid_index_);
            recv_callback_(this);
        }

        if (next_send_valid_index_ > 0)
        {
            ssize_t n = ::send(socket_fd_, outbound_data_.data(), next_send_valid_index_, MSG_DONTWAIT | MSG_NOSIGNAL);

            logger_.log("%:% %() % send socket:% len:%\n",
                        __FILE__, __LINE__, __FUNCTION__,
                        getCurrentTimeStr(&time_str_), socket_fd_, n);
        }
        next_send_valid_index_ = 0;

        return n_rcv > 0;
    }

    auto McastSocket::send(const char *data, size_t len) -> bool
    {
        memcpy(outbound_data_.data() + next_send_valid_index_, data, len);
        next_send_valid_index_ += len;
        ASSERT(next_send_valid_index_ < McastBufferSize,
               "Mcast socket buffer filled up and sendAndRecv() not called.");
    }
}