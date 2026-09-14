#ifndef AERIO_DETAIL_OPERATION_QUEUE_HPP
#define AERIO_DETAIL_OPERATION_QUEUE_HPP

#include "descriptor_operation.hpp"

namespace aerio {
namespace detail {

class operation_queue
{
public:
    operation_queue() 
        : _head(nullptr), _tail(nullptr) 
    {}

    bool is_enqueued(descriptor_operation *desc_op)
    {
        return desc_op->_next != nullptr || _tail == desc_op;
    }

    void push(descriptor_operation *desc_op)
    {
        desc_op->_next = nullptr;
        if (_tail) {
            _tail->_next = desc_op;
            _tail = desc_op;
        } else {
            _head = _tail = desc_op;
        }
    }

    bool empty()
    {
        return _head == nullptr;
    }

    descriptor_operation * front()
    {
        return _head;
    }

    void pop()
    {
        if (_head) {
            auto *tmp = _head;
            _head = _head->_next;
            if (!_head)
                _tail = nullptr;
            tmp->_next = nullptr;
        }
    }

    descriptor_operation *front_pop()
    {
        auto *tmp = front();
        pop();
        return tmp;
    }

    
private:
    descriptor_operation *_head, *_tail;
};

}
}


#endif