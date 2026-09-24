#ifndef AERIO_NET_DETAIL_SOCKET_HPP
#define AERIO_NET_DETAIL_SOCKET_HPP


#include <aerio/core/operation.hpp>
#include <aerio/net/protocol.hpp>
#include <aerio/core/io_context.hpp>
#include <aerio/net/endpoint.hpp>

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
namespace net {
namespace detail {

class socket {
public:
    socket(aerio::core::io_context& ctx, const net::protocol_type &protocol, int flags = 0, int fd = -1)
        : _protocol(protocol),
        _is_nonblock(false),
        _ctx(ctx),
        _fd(fd)
    {
        auto type = _protocol.type() | (flags & (SOCK_NONBLOCK | SOCK_CLOEXEC));
        if (_fd < 0)
            _fd = ::socket(_protocol.family(), type, _protocol.protocol());
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

    int bind(const aerio::net::endpoint& ep)
    {
        if ( _protocol.family() != ep.family()) {
            errno = EAFNOSUPPORT;
            return -1;
        }

        return ::bind(_fd, ep.sockaddr_ptr(), ep.size());
    }

    int listen()
    {
        return ::listen(_fd, 4096);
    }

    int accept(aerio::net::endpoint &ep)
    {
        if ( _protocol.family() != ep.family()) {
            errno = EAFNOSUPPORT;
            return -1;
        }

        socklen_t addrlen = static_cast<socklen_t>(ep.size());
        return ::accept(_fd, ep.sockaddr_ptr(), &addrlen);
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

    aerio::core::io_context& context() noexcept
    {
        return _ctx;
    }

    core::operation& op() noexcept
    {
        return _op;
    }

    const net::protocol_type& protocol() const noexcept
    {
        return _protocol;
    }
    

private:
    int _fd;
    const net::protocol_type &_protocol;
    bool _is_nonblock;
    aerio::core::io_context &_ctx;

    core::operation _op;
};

}
}
}


#endif