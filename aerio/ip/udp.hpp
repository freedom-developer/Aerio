#ifndef AERIO_IP_UDP_HPP
#define AERIO_IP_UDP_HPP

#include "basic_endpoint.hpp"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace aerio {
namespace ip {

class udp {
public:
    static udp v4()
    {
        return udp(AF_INET);
    }
    static udp v6()
    {
        return udp(AF_INET6);
    }
    
    int family() const { return _family; }
    int type() const { return SOCK_DGRAM; }
    int protocol() const { return IPPROTO_UDP; }

    typedef basic_endpoint<udp> endpoint;

private:
    udp(int family) : _family(family) {}
    int _family;
};

}
}

#endif