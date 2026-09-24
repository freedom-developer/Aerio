#ifndef AERIO_SOCKET_LISTENER_HPP
#define AERIO_SOCKET_LISTENER_HPP

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <sys/epoll.h>
#include <system_error>
#include <memory>
#include <functional>

#include <aerio/net/detail/socket.hpp>
#include <aerio/net/endpoint.hpp>
#include <type_traits>

namespace aerio {
namespace net {

class listener : detail::socket{
public:
    using listener_callback_t = std::function<void(int, std::shared_ptr<detail::socket>)>;

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

    void async_accept(listener_callback_t cb)
    {
        _cb = cb;

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
                    
                    auto nsk =  std::make_shared<detail::socket>(sk->context(), sk->protocol(), 0, fd);
                    if (sk->_cb)
                        sk->_cb(0, nsk);
                    
                } else {
                    if (errno == EINTR)
                        continue;

                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                    else
                        sk->_cb(errno, nullptr);
                }
            }
        } else {
            // EPOLLERR, ERPLLHUP事件处理
            std::cout << "accept socket meet EVENT " << op.revents << "\n"; 
        }
    }

private:
    listener_callback_t _cb;
};

}
}

#endif
