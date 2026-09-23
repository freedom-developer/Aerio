#ifndef AERIO_CORE_OPERATION_HPP
#define AERIO_CORE_OPERATION_HPP

#include <aerio/util/list.hpp>

namespace aerio {
namespace core {

typedef void (*op_func_type)(void *);

struct operation {
    operation() : fd(-1), func(nullptr), data(nullptr), revents(0)
    {
        util::INIT_LIST_HEAD(&node);
    }
    operation(int _fd, op_func_type _func, void *_data = nullptr)
        : fd(_fd), func(_func), data(_data), revents(0)
    {
        util::INIT_LIST_HEAD(&node);
    }

    int fd;
    int revents;
    op_func_type func;
    void *data;

    util::list_head node;
};

class op_queue {
public:
    op_queue()
    {
        util::INIT_LIST_HEAD(&_head);
    }

    void push(operation *op)
    {
        util::list_add_tail(&op->node, &_head);
    }

    bool empty()
    {
        return util::list_empty(&_head);
    }

    operation *front_del()
    {
        if (empty())
            return nullptr;
        auto *op = list_first_entry(&_head, operation, node);
        util::list_del(&op->node);

        return op;
    }

private:
    util::list_head _head;
};

}
}

#endif