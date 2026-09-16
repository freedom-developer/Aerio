#ifndef AERIO_DETAIL_FD_OPQ_HPP
#define AERIO_DETAIL_FD_OPQ_HPP

#include "fd_operation.hpp"
#include <cstddef>

namespace aerio {
namespace detail {

class fd_opq
{
public:
    fd_opq() 
        : _head(nullptr), _tail(nullptr) 
    {}

    bool is_enqueued(fd_operation *desc_op)
    {
        return desc_op->_next != nullptr || _tail == desc_op;
    }

    void push(fd_operation *desc_op)
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

    fd_operation * front()
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

    fd_operation *front_pop()
    {
        auto *tmp = front();
        pop();
        return tmp;
    }


    void splice(fd_opq &o)
    {
        if (_tail) 
            _tail->_next = o._head;
        else 
            _head = o._head;
        _tail = o._tail;

        o._head = o._tail = nullptr;
    }

    
private:
    fd_operation *_head, *_tail;
};

}
}


#endif