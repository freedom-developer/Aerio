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
    tcp::endpoint ep(tcp::v4(), 1203, "127.0.0.1");

    aerio::core::io_context ctx;
    tcp::listener listener(tcp::v4(), ctx, ep);

    // listener.async_accept

    ctx.run();
    
    return 0;
}