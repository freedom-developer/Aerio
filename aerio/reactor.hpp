#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <sys/epoll.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>

namespace aerio {

class reactor {
public:
    reactor() : _epoll_fd(::epoll_create1(EPOLL_CLOEXEC))
    {
        if (_epoll_fd < 0) {
            const int err = errno;
            throw std::system_error(err, std::system_category(), "epoll_cretae");
        }
    }

    ~reactor()
    {
        if (_epoll_fd > 0)
            close(_epoll_fd);
    }
    
private:
    int _epoll_fd;
};

}

#endif
