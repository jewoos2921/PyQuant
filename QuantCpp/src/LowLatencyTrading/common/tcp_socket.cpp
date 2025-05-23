#include "tcp_socket.hpp"

namespace QuantCpp::Common
{
    auto TCPSocket::connect(const std::string &ip, const std::string &iface, int port, bool is_listening) -> int
    {
        const SocketCfg socket_cfg{ip, iface, port, false, is_listening, true};
        socket_fd_ = createSocket(logger_, socket_cfg);

        socket_attrib_.sin_addr.s_addr = INADDR_ANY;
        socket_attrib_.sin_family = AF_INET;
        socket_attrib_.sin_port = htons(port);

        return socket_fd_;
    }

    auto TCPSocket::sendAndRecv() noexcept -> int
    {
        char ctrl[CMSG_SPACE(sizeof(struct timeval))];
        auto cmsg = reinterpret_cast<struct cmsghdr *>(&ctrl);

        iovec iov{inbound_data_.data() + next_rcv_valid_index_, TCPBufferSize - next_rcv_valid_index_};
        msghdr msg{&socket_attrib_, sizeof(socket_attrib_),
                   &iov, 1, ctrl, sizeof(ctrl), 0};

        const auto read_size = recvmsg(socket_fd_, &msg, MSG_DONTWAIT);
        if (read_size > 0)
        {
            next_rcv_valid_index_ += read_size;

            Nanos kernel_time = 0;
            timeval time_kernel;
            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMPNS && cmsg->cmsg_len == CMSG_LEN(sizeof(time_kernel)))
            {
                memcpy(&time_kernel, CMSG_DATA(cmsg), sizeof(time_kernel));
                kernel_time = time_kernel.tv_sec * NANOS_TO_SECS + time_kernel.tv_usec * NANOS_TO_MICROS;
            }
            const auto user_time = getCurrentNanos();

            logger_.log("%:% %() % read socket:% len:% utime:% ktime:% diff:%\n", __FILE__, __LINE__, __FUNCTION__,
                        getCurrentTimeStr(&time_str_), socket_fd_, next_rcv_valid_index_, user_time, kernel_time, (user_time - kernel_time));
            recv_callback_(this, kernel_time);
        }

        if (next_send_valid_index_ > 0)
        {
            const auto n = ::send(socket_fd_, outbound_data_.data(), next_send_valid_index_, MSG_DONTWAIT | MSG_NOSIGNAL);
            logger_.log("%:% %() % send socket:% len:%\n",
                        __FILE__, __LINE__, __FUNCTION__,
                        getCurrentTimeStr(&time_str_), socket_fd_, n);
        }
        next_send_valid_index_ = 0;
        return read_size > 0;
    }
    
    auto TCPSocket::send(const void *data, size_t len) noexcept -> void
    {
        memcpy(outbound_data_.data() + next_send_valid_index_, data, len);
        next_send_valid_index_ += len;
    }
}