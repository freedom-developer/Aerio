#ifndef AERIO_NET_PROTOCOL_HPP
#define AERIO_NET_PROTOCOL_HPP

#include <netinet/in.h>
#include <sys/socket.h>

namespace aerio {
namespace net {

class protocol_type {
public:
    protocol_type(int family, int type, int protocol)
        : _family(family), _type(type), _protocol(protocol)
    {}

    int family() const noexcept { return _family; }
    int type() const noexcept { return _type;}
    int protocol() const noexcept { return _protocol; }

private:
    int _family, _type, _protocol;
};

static protocol_type tcp_v4(AF_INET, SOCK_STREAM, IPPROTO_TCP);
static protocol_type tcp_v6(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

static protocol_type udp_v4(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
static protocol_type udp_v6(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);


}
}

#endif