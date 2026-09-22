#ifndef AERIO_CORE_EVENT_HPP
#define AERIO_CORE_EVENT_HPP

#include <sys/epoll.h>

#include <system_error>

#include <aerio/util/list.hpp>

namespace aerio {
namespace core {

struct event {
    using callback_type = void (*)(event*);

    event(int fd, callback_type do_event = nullptr)
        : _fd(fd),
          _revents(0),
          _do_event(do_event)
    {
        INIT_LIST_HEAD(&list_node);
    }

    int _fd;
    int _revents;
    callback_type _do_event;

    util::list_head list_node;
};

}
}

#endif
