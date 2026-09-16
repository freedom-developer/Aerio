#ifndef AERIO_IO_CONTEXT_HPP
#define AERIO_IO_CONTEXT_HPP

#include "scheduler.hpp"
namespace aerio {

class io_context {
public:
    io_context() {}

    scheduler& get_sched() { return sched; }

private:
    scheduler sched;
};

}

#endif