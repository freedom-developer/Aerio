#include "net/protocol.hpp"
#include <exception>
#include <iostream>



#include <aerio/aerio.hpp>

int main(void)
{
    std::cout << "Hello Aerio" << std::endl;
    try {
        aerio::net::endpoint ep(aerio::net::tcp_v4, 1203, "127.0.0.1");

        aerio::core::io_context ctx;
        aerio::net::listener listener(ctx, ep);

        listener.async_accept();

        ctx.run();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    
    return 0;
}