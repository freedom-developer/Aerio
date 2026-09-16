#ifndef AERIO_SCHEDULER_HPP
#define AERIO_SCHEDULER_HPP

#include "detail/fd_operation.hpp"
#include "detail/fd_opq.hpp"
#include "reactor.hpp"

#include <cstddef>
#include <mutex>
#include <pthread.h>
#include <cassert>

namespace aerio {

class scheduler
{
public:
    scheduler()
        : _stopped(false), 
        _actor(),
        _mtx(PTHREAD_MUTEX_INITIALIZER),
        _cond(PTHREAD_COND_INITIALIZER)
    {

    }


    int run()
    {
        for (; do_run_one(); )
            ;

        return 0;
    }

    reactor& get_actor() { return _actor; }

private:
    /* 可能多线程调用此接口 */
    int do_run_one()
    {
        /* 线程私有的操作队列 */
        detail::fd_opq private_op_queue;

        pthread_mutex_lock(&_mtx);

        while (!_stopped) {
            detail::fd_operation *desc_op = nullptr;

            /* 互斥地从全局操作队列中获取一个操作, pthread_cond_wait进入休眠前会unlock(_mtx) */
            while (_op_queue.empty()) 
                pthread_cond_wait(&_cond, &_mtx);
            desc_op = _op_queue.front_pop();
            if (!_op_queue.empty())
                pthread_cond_signal(&_cond); // 一次唤醒一个线程
            pthread_mutex_unlock(&_mtx);

            assert(("The descriptor operation is nullptr.", desc_op != nullptr));

            if (desc_op == &_tag_op) { // 产生操作
                _actor.run(private_op_queue);

                pthread_mutex_lock(&_mtx);
                if (!private_op_queue.empty()) {
                    _op_queue.splice(private_op_queue);
                }
                _op_queue.push(&_tag_op);
                pthread_cond_signal(&_cond); // 一次只唤醒一个线程
            } else { // 消耗操作
                // desc_op->
            }
        }

        return 0; // -> 在run函数中下次不再执行
    }

    bool _stopped;
    detail::fd_opq _op_queue;
    reactor _actor;
    detail::fd_operation _tag_op{};
    ::pthread_mutex_t _mtx;
    ::pthread_cond_t _cond;
};

}

#endif