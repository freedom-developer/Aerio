#ifndef AERIO_IO_CONTEXT_HPP
#define AERIO_IO_CONTEXT_HPP

#include <cassert>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

#include <system_error>

namespace aerio {
namespace core {

class io_context {
public:
    io_context()
        : _epfd(epoll_create1(EPOLL_CLOEXEC))
    {
        if (_epfd < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "epoll_create1(EPOLL_CLOEXEC)");
        }

    }

    ~io_context()
    {
        if (_epfd >= 0)
            close(_epfd);
    }

    void run()
    {
        assert(_epfd >= 0);

        epoll_event evs[128];
        
        while (!_stop) {
            auto ready = epoll_wait(_epfd, evs, 128, -1);
            if (ready < 0) {
                const auto err = errno;
                throw std::system_error(err, std::system_category(), "epoll_wait");
            }
            
        }

    }

private:
    int _epfd;
    bool _stop;

};

}
}

#endif