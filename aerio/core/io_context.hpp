#ifndef AERIO_IO_CONTEXT_HPP
#define AERIO_IO_CONTEXT_HPP

#include "core/operation.hpp"
#include <cassert>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

#include <aerio/core/event.hpp>
#include <aerio/util/list.hpp>

#include <system_error>
#include <list>
#include <iostream>

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

        _opq.push(&_epfd_op); 
    }

    ~io_context()
    {
        if (_epfd >= 0)
            close(_epfd);
    }

    void run()
    {
        assert(_epfd >= 0);
        event *ev;
        while (!_stop) {
            auto *op = _opq.front_del();
            if (op) {
                if (op == &_epfd_op) {
                    do_epoll_wait();

                    // 谁取的，谁负责重新压入
                    _opq.push(&_epfd_op);
                } else {
                    op->func(op->data);
                }
            } else {
                // todo: 等待
            }
        }
    }

    void do_epoll_wait()
    {
        epoll_event evs[128];
        std::cout << "do_epoll_wait" << std::endl;
        auto ready = epoll_wait(_epfd, evs, 128, -1);
        if (ready < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "epoll_wait");
        }

        std::cout << "some event happened. " << std::endl;
        for (auto i = 0; i < ready; i++) {
            auto *op = static_cast<operation*>(evs[i].data.ptr);
            op->revents = evs[i].events;
            _opq.push(op);
        }
    }

    void epoll_add_fd(int fd, int events, operation *op)
    {
        epoll_event epev;
        epev.events = events;
        epev.data.ptr = static_cast<void *>(op);

        std::cout << "add fd into epfd" << std::endl;
        auto ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &epev);
        if (ret < 0) {
            auto err = errno;
            throw std::system_error(err, std::system_category(), "epoll_ctl(EPOLL_CTL_ADD)");
        }
    }

private:
    int _epfd;
    bool _stop;
    operation _epfd_op;

    op_queue _opq;    
};

}
}

#endif