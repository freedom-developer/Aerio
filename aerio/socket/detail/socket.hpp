#ifndef AERIO_SOCKET_DETAIL_SOCKET_HPP
#define AERIO_SOCKET_DETAIL_SOCKET_HPP

#include "core/event.hpp"
#include <aerio/ip/detail/endpoint.hpp>
#include <aerio/core/io_context.hpp>

#include <asm-generic/errno.h>
#include <cstddef>
#include <netinet/in.h>
#include <new>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <cerrno>
#include <system_error>

namespace aerio {
namespace socket {
namespace detail {

class socket {
public:
    socket(aerio::core::io_context& ctx, int family, int type, int protocol = 0)
        : _family(family),
        _type(type),
        _protocol(protocol),
        _is_nonblock(false),
        _ctx(ctx)
    {
        _fd = ::socket(_family, _type, _protocol);
        if (_fd < 0) {
            auto err = errno;
            throw std::system_error{err, std::system_category(), "epoll_cretae"};
        }
        if (type & SOCK_NONBLOCK)
            _is_nonblock = true;
    }

    ~socket()
    {
        if (_fd > 0)
            ::close(_fd);
    }

    int bind(const aerio::ip::detail::endpoint& ep)
    {
        if (_family != ep.family()) {
            errno = EAFNOSUPPORT;
            return -1;
        }

        if (_family == AF_INET) {
            const auto& addr = ep.v4();
            return ::bind(_fd, (const sockaddr *)&addr, sizeof(addr));    
        } else if (_family == AF_INET6) {
            const auto& addr = ep.v6();
            return ::bind(_fd, (const sockaddr*)&addr, sizeof(addr));
        } else {
            errno = EAFNOSUPPORT;
            return -1;
        }
    }

    int listen()
    {
        return ::listen(_fd, 4096);
    }

    void async_accept()
    {
        set_nonblock();

        // 将此_fd加入epollfd中，并设置回调
        core::event *ev = new core::event(_fd, socket::accept);
        _ctx.epoll_add_fd(_fd, EPOLLIN, ev);
    }

    static void accept(core::event *ev)
    {
        int fd;
        sockaddr_in addr4;
        sockaddr_in6 addr6;

        while ((fd = ::accept(ev->_fd, NULL, NULL)) > 0) {
            // accept成功场景
            
        }

        // 处理accept失败场景
    }

    void set_nonblock()
    {
        if (_is_nonblock)
            return;

        auto fl = fcntl(_fd, F_GETFL, NULL);
        if (fl < 0) {
            auto err = errno;
            throw std::system_error(err, std::system_category(), "fcntl(F_GETFL) failed");
        }
        fl |= O_NONBLOCK;
        auto ret = fcntl(_fd, F_SETFL, &fl);
        if (ret < 0) {
            auto err = errno;
            throw std::system_error(err, std::system_category(), "fcntl(F_SETFL) failed");
        }

        _is_nonblock = true;
    }

private:
    int _fd;
    int _family, _type, _protocol;
    bool _is_nonblock;
    aerio::core::io_context &_ctx;
};

}
}
}


#endif