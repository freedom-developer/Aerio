#ifndef AERIO_IO_CONTEXT_HPP
#define AERIO_IO_CONTEXT_HPP

#include <cassert>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

#include <aerio/core/event.hpp>
#include <aerio/util/list.hpp>

#include <system_error>
#include <list>

namespace aerio {
namespace core {

class io_context {
public:
    io_context()
        : _epfd(epoll_create1(EPOLL_CLOEXEC)),
        _ep_event(_epfd)
    {
        if (_epfd < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "epoll_create1(EPOLL_CLOEXEC)");
        }

        INIT_LIST_HEAD(&_event_head);
        util::list_add(&_ep_event.list_node, &_event_head);        
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
            list_for_each_entry(ev, &_event_head, list_node) {
                if (ev == &_ep_event) {
                    do_epoll_wait();
                } else {
                    ev->_do_event(ev);
                }
            }

            // 刷新队列
            util::INIT_LIST_HEAD(&_event_head);
            util::list_add(&_ep_event.list_node, &_event_head);

        }
    }

    void do_epoll_wait()
    {
        epoll_event evs[128];
        auto ready = epoll_wait(_epfd, evs, 128, -1);
        if (ready < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "epoll_wait");
        }

        for (auto i = 0; i < ready; i++) {
            auto evp = static_cast<event*>(evs[i].data.ptr);
            util::list_add(&evp->list_node, &_event_head);
        }
    }

    void push_event(event *ev)
    {
        util::list_add(&ev->list_node, &_event_head);
    }

    void epoll_add_fd(int fd, int events, event *ev)
    {
        epoll_event epev;
        epev.events = events;
        epev.data.ptr = static_cast<void *>(ev);

        auto ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &epev);
        if (ret < 0) {
            auto err = errno;
            throw std::system_error(err, std::system_category(), "epoll_ctl(EPOLL_CTL_ADD)");
        }
    }

private:
    int _epfd;
    bool _stop;
    event _ep_event;
    util::list_head _event_head;
    
};

}
}

#endif