#ifndef AERIO_DETAIL_FD_OPERATION_HPP
#define AERIO_DETAIL_FD_OPERATION_HPP

#include <cstddef>
#include <cstdint>

#include <pthread.h>

namespace aerio {
namespace detail {

class operation_queue;

class fd_operation
{
public:
    friend class fd_opq;
    friend class fd_op_pool;
    
    fd_operation() 
        : _next(nullptr),
        _ready_events(0),
        _register_events(0),
        _fd(-1)
    {}

    fd_operation *_next;
    uint32_t _ready_events;
    uint32_t _register_events;
    int _fd;
};

class fd_op_pool
{
public:
    fd_op_pool(std::size_t max = 1024)
        : _fl_mtx(PTHREAD_MUTEX_INITIALIZER),
        _freelist(nullptr),
        _max(max),
        _size(0)
    {}

    ~fd_op_pool()
    {
        pthread_mutex_lock(&_fl_mtx);
        auto *fd_op = _freelist;
        _freelist = nullptr;
        while (fd_op) {
            auto *tmp = fd_op->_next;
            delete fd_op;
            fd_op = tmp;
        }
    }

    // 可能抛出std::bad_alloc &异常
    fd_operation *alloc()
    {
        fd_operation *fd_op = nullptr;
        pthread_mutex_lock(&_fl_mtx);
        if (_freelist) {
            fd_op = _freelist;
            _freelist = _freelist->_next;
        }
        pthread_mutex_unlock(&_fl_mtx);
        if (!fd_op)
            fd_op = new fd_operation;
        
        if (fd_op)
            fd_op->_next = nullptr;
        
        return fd_op;
    }

    void free(fd_operation *fd_op)
    {
        if (_size >= _max)
            delete fd_op;
        else {
            pthread_mutex_lock(&_fl_mtx);
            fd_op->_next = _freelist;
            _freelist = fd_op;
            pthread_mutex_unlock(&_fl_mtx);
        }
    }


private:
    fd_operation *_freelist;
    pthread_mutex_t _fl_mtx;
    std::size_t _max, _size;
};

}
}

#endif