#ifndef AERIO_SOCKET_DETAIL_SOCKET_HPP
#define AERIO_SOCKET_DETAIL_SOCKET_HPP

#include "core/event.hpp"
#include "core/operation.hpp"
#include <aerio/ip/detail/endpoint.hpp>
#include <aerio/core/io_context.hpp>
// #include <aerio/ip/tcp.hpp>

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cstddef>
#include <netinet/in.h>
#include <new>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <cerrno>
#include <system_error>
#include <iostream>
#include <string>
#include <string.h>

namespace aerio {
namespace socket {
namespace detail {

class socket {
public:
    socket(aerio::core::io_context& ctx, int family, int type, int protocol = 0, int fd = -1)
        : _family(family),
        _type(type),
        _protocol(protocol),
        _is_nonblock(false),
        _ctx(ctx),
        _op(nullptr),
        _fd(fd)
    {
        if (_fd < 0)
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
        if (_op)
            delete _op;
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

    int accept(aerio::ip::detail::endpoint &ep)
    {
        int fd;
        socklen_t addrlen;
        if (_family == AF_INET) {
            addrlen = sizeof(ep.v4());
            fd = ::accept(_fd, (sockaddr *)&ep.v4(), &addrlen);
        } else {
            addrlen = sizeof(ep.v6());
            fd = ::accept(_fd, (sockaddr *)&ep.v6(), &addrlen);
        }
        return fd;
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
        auto ret = fcntl(_fd, F_SETFL, fl);
        if (ret < 0) {
            auto err = errno;
            throw std::system_error(err, std::system_category(), "fcntl(F_SETFL) failed, " + std::string(strerror(err)));
        }

        _is_nonblock = true;
    }

    int fd()
    {
        return _fd;
    }

    aerio::core::io_context& context()
    {
        return _ctx;
    }

    int family()
    {
        return _family;
    }

private:
    int _fd;
    int _family, _type, _protocol;
    bool _is_nonblock;
    aerio::core::io_context &_ctx;

    core::operation *_op;
};

}
}
}


#endif