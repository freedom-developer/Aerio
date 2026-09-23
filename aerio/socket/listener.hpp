#ifndef AERIO_SOCKET_LISTENER_HPP
#define AERIO_SOCKET_LISTENER_HPP

#include "ip/detail/endpoint.hpp"
#include "socket/detail/socket.hpp"
#include <aerio/core/operation.hpp>
#include <aerio/ip/basic_endpoint.hpp>
#include <aerio/socket/basic_socket.hpp>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <system_error>

namespace aerio {
namespace socket {

template <typename ProtocolType>
class listener : public basic_socket<ProtocolType>{
public:
    using endpoint_type = aerio::ip::basic_endpoint<ProtocolType>;

    listener(const ProtocolType& protocol,
             aerio::core::io_context& ctx,
             const endpoint_type& ep)
        : basic_socket<ProtocolType>(protocol, ctx)
    {
        auto ret = this->bind(ep);
        if (ret < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "bind");
        }

        ret = this->listen();
        if (ret < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "listen");
        }
    }

    void async_accept()
    {
        this->set_nonblock();

        // 将此_fd加入epollfd中，并设置回调
        auto &op = this->op();
        op.fd = this->fd();
        op.func = listener<ProtocolType>::listener_accept;
        op.data = this;
        this->context().epoll_add_fd(this->fd(), EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET, &op);
    }

    static void listener_accept(void *data)
    {
        auto *sk = static_cast<listener<ProtocolType> *>(data);
        auto& op = sk->op();
        if (op.revents & EPOLLIN) {
            while (1) {
                ip::detail::endpoint ep(sk->family());
                auto fd = sk->accept(ep);
                if (fd >= 0) {
                    std::cout << "someone connect to this server" << std::endl;
                    detail::socket sock(sk->context(), )
                } else {
                    if (errno == EINTR)
                        continue;

                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                }
            }
        } else {
            // EPOLLERR, ERPLLHUP事件处理
            std::cout << "accept socket meet EVENT " << op.revents << "\n"; 
        }
    }
};

}
}

#endif
