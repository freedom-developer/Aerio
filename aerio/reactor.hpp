#ifndef AERIO_REACTOR_HPP
#define AERIO_REACTOR_HPP

#include <sys/epoll.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>

#include <queue>

#include "detail/descriptor_operation.hpp"
#include "detail/operatoion_queue.hpp"

namespace aerio {

class reactor 
{
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

    void run(detail::operation_queue &op_queue)
    {
        epoll_event events[128];
        auto num_events = epoll_wait(_epoll_fd, events, 128, -1);
        for(int i = 0; i < num_events; i++) {
            void *ptr = events[i].data.ptr;
            
            detail::descriptor_operation *desc_op = static_cast<detail::descriptor_operation*>(ptr);
            if (!op_queue.is_enqueued(desc_op)) {
                desc_op->set_ready_events(events[i].events);
                op_queue.push(desc_op);
            } else {
                desc_op->add_ready_events(events[i].events);
            }
        
        }

    }
    

private:
    int _epoll_fd;
};

}

#endif
