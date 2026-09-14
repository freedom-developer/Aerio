#ifndef AERIO_DETAIL_DESCRIPTOR_OPERATION_HPP
#define AERIO_DETAIL_DESCRIPTOR_OPERATION_HPP

#include <cstdint>

namespace aerio {
namespace detail {

class operation_queue;

class descriptor_operation
{
public:
    friend class operation_queue;
    
    descriptor_operation() : _next(nullptr) {}

    void set_ready_events(uint32_t events)
    {
        _ready_events = events;
    }

    void add_ready_events(uint32_t events)
    {
        _ready_events |= events;
    }

private:
    descriptor_operation *_next;
    uint32_t _ready_events;

};

}
}

#endif