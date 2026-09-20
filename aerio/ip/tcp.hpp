#ifndef AERIO_IP_TCP_HPP
#define AERIO_IP_TCP_HPP

#include "socket/listener.hpp"
#include <aerio/ip/basic_endpoint.hpp>
#include <aerio/socket/basic_socket.hpp>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <aerio/socket/basic_socket.hpp>

namespace aerio {
namespace ip {

class tcp {
public:
    static tcp v4()
    {
        return tcp(AF_INET);
    }
    static tcp v6()
    {
        return tcp(AF_INET6);
    }
    
    int family() const { return _family; }
    int type() const { return SOCK_STREAM; }
    int protocol() const { return IPPROTO_TCP; }

    typedef basic_endpoint<tcp> endpoint;
    typedef aerio::socket::basic_socket<tcp> socket;
    typedef aerio::socket::listener<tcp> listener;

private:
    tcp(int family) : _family(family) {}
    int _family;
};

}
}

#endif