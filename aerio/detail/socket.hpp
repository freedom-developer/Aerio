#ifndef AERIO_DETAIL_SOCKET_HPP
#define AERIO_DETAIL_SOCKET_HPP

#include "../io_context.hpp"
#include "fd_operation.hpp"

#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>

namespace aerio {
namespace detail {

class socket {
public:
    socket(aerio::io_context &ctx, int family = AF_INET, int type = SOCK_STREAM, int protocol = 0)
        : _ctx(ctx),
        _family(family),
        _type(type),
        _protocol(protocol)
    {
        _fd = ::socket(_family, _type, _protocol);
        if (_fd < 0) {
            auto err = errno;
            throw std::system_error{err, std::system_category(), "epoll_cretae"};
        }
        
        _ctx.get_sched().get_actor().register_fd(_fd, _fd_op);

    }

    ~socket()
    {
        if (_fd_op)
            _ctx.get_sched().get_actor().unregister_fd(_fd_op);
        
        if (_fd > 0)
            ::close(_fd);
    }

private:
    int _fd;
    int _family, _type, _protocol;
    aerio::io_context &_ctx;

    fd_operation *_fd_op;
};

}
}


#endif