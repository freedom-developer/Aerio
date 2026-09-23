#include <exception>
#include <iostream>

#include "aerio/ip/basic_endpoint.hpp"
#include "aerio/ip/tcp.hpp"
#include "aerio/ip/udp.hpp"
#include "core/io_context.hpp"

using namespace aerio;
using namespace aerio::ip;



int main(void)
{
    std::cout << "Hello Aerio" << std::endl;
    try {
        tcp::endpoint ep(v4, 1203, "127.0.0.1");

        aerio::core::io_context ctx;
        tcp::listener listener(v4, ctx, ep);

        listener.async_accept();

        ctx.run();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    
    return 0;
}