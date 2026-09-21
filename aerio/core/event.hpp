#ifndef AERIO_CORE_EVENT_HPP
#define AERIO_CORE_EVENT_HPP

#include <sys/epoll.h>

#include <functional>
#include <system_error>

namespace aerio {
namespace core {

class event {
public:
    event(int epfd, int fd, int events, std::function<void(event *)> do_event)
        : _fd(fd),
          _events(events),
          _revents(0),
          _do_event(do_event),
          _prev(nullptr),
          _next(nullptr)
    {
        if (epfd < 0 || fd < 0)
            throw std::invalid_argument("construct event with error arguments");
        
        epoll_event ev;
        ev.events = events;
        ev.data.ptr = this;
        if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _fd, &ev) < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "epoll_ctl(EPOLL_CTL_ADD)");
        }
    }

private:
    int _fd, _epfd;
    int _events, _revents;
    std::function<void(event *)> _do_event;
    event *_prev, *_next;
};

class event_queue {
public:
    struct event_head {
        event *prev{nullptr}, *next{nullptr};
        bool empty()
        {
            return prev == next && prev == nullptr;
        }

        void insert_tail(event *ev)
        {
            
        }
    };

    event_queue()
    {}

    bool empty()
    {
        return _eq_head.empty();
    }
    


private:
    event_head _eq_head;
};

}
}

#endif