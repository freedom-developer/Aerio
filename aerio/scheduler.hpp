#ifndef AERIO_SCHEDULER_HPP
#define AERIO_SCHEDULER_HPP

#include "detail/descriptor_operation.hpp"
#include "detail/operatoion_queue.hpp"
#include "reactor.hpp"

#include <mutex>

namespace aerio {

class scheduler
{
public:
    scheduler()
        : _stopped(false), _actor()
    {

    }


    int run()
    {
        for (; do_run_one(); )
            ;

        return 0;
    }

private:
    /* 可能多线程调用此接口 */
    int do_run_one()
    {
        while (!_stopped) {
            if (!_op_queue.empty()) {
                auto *desc_op= _op_queue.front_pop();
                if (desc_op == &_tag_op) {
                    _actor.run(_op_queue);
                }
            } else {

            }
        }

        return 0; // -> 在run函数中下次不再执行
    }


    bool _stopped;
    detail::operation_queue _op_queue;
    reactor _actor;
    detail::descriptor_operation _tag_op{};
    std::mutex _mtx;

};

}

#endif