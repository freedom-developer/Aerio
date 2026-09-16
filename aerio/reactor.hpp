#ifndef AERIO_REACTOR_HPP
#define AERIO_REACTOR_HPP

#include <cstddef>
#include <pthread.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>

#include "detail/fd_operation.hpp"
#include "detail/fd_opq.hpp"

namespace aerio {

class reactor 
{
public:
    reactor() 
        : _epoll_fd(::epoll_create1(EPOLL_CLOEXEC)),
        _fd_op_pool()
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

    void run(detail::fd_opq &op_queue) noexcept
    {
        epoll_event events[128];
        auto num_events = epoll_wait(_epoll_fd, events, 128, -1);
        for(int i = 0; i < num_events; i++) {
            void *ptr = events[i].data.ptr;
            
            detail::fd_operation *desc_op = static_cast<detail::fd_operation*>(ptr);
            if (!op_queue.is_enqueued(desc_op)) {
                desc_op->_ready_events = events[i].events;
                op_queue.push(desc_op);
            } else {
                desc_op->_ready_events |= events[i].events;
            }
        
        }

    }

    // 可能抛出std::bad_alloc &异常
    int register_fd(int fd, detail::fd_operation* &fd_op_ret)
    {
        auto *fd_op = _fd_op_pool.alloc();

        epoll_event ev;
        ev.data.ptr = static_cast<void *>(fd_op);
        ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
        fd_op->_fd = fd;
        fd_op->_ready_events = 0;
        fd_op->_register_events = ev.events;
        
        auto result = ::epoll_ctl(_epoll_fd, fd, EPOLL_CTL_ADD, &ev);
        if (result)
            _fd_op_pool.free(fd_op);
        fd_op_ret = fd_op;
        
        return result;
    }

    void unregister_fd(detail::fd_operation *fd_op)
    {
        _fd_op_pool.free(fd_op);
    }
    

private:
    int _epoll_fd;
    detail::fd_op_pool _fd_op_pool;

};

}

#endif
