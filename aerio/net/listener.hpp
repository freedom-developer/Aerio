#ifndef AERIO_SOCKET_LISTENER_HPP
#define AERIO_SOCKET_LISTENER_HPP

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <system_error>

#include <aerio/net/detail/socket.hpp>
#include <aerio/net/endpoint.hpp>

namespace aerio {
namespace net {

class listener : detail::socket{
public:

    listener(aerio::core::io_context& ctx, const endpoint& ep)
        : detail::socket(ctx, ep.protocol())
    {
        auto ret = bind(ep);
        if (ret < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "bind");
        }

        ret = listen();
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
        op.fd = fd();
        op.func = listener::listener_accept;
        op.data = this;
        this->context().epoll_add_fd(this->fd(), EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET, &op);
    }

    static void listener_accept(void *data)
    {
        auto *sk = static_cast<listener*>(data);
        auto& op = sk->op();
        if (op.revents & EPOLLIN) {
            while (1) {
                endpoint ep(sk->protocol());
                auto fd = sk->accept(ep);
                if (fd >= 0) {
                    std::cout << ep.address_port() << " to this server" << std::endl;
                    
                    detail::socket sock(sk->context(), sk->protocol(), 0, fd);
                    

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
