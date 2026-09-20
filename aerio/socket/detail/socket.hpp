#ifndef AERIO_SOCKET_DETAIL_SOCKET_HPP
#define AERIO_SOCKET_DETAIL_SOCKET_HPP

#include <aerio/ip/detail/endpoint.hpp>
#include <asm-generic/errno.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>

namespace aerio {
namespace socket {
namespace detail {

class socket {
public:
    socket(int family, int type, int protocol = 0)
        : _family(family),
        _type(type),
        _protocol(protocol)
    {
        _fd = ::socket(_family, _type, _protocol);
        if (_fd < 0) {
            auto err = errno;
            throw std::system_error{err, std::system_category(), "epoll_cretae"};
        }
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

private:
    int _fd;
    int _family, _type, _protocol;
};

}
}
}


#endif