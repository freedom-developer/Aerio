#ifndef AERIO_IP_DETAIL_ENDPOINT_HPP
#define AERIO_IP_DETAIL_ENDPOINT_HPP

#include <cerrno>
#include <cstdint>
#include <stdexcept>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <system_error>

namespace aerio {
namespace ip {
namespace detail {

class endpoint {
public:
    endpoint(int family = AF_INET, uint16_t port = 0, const std::string &addr = "")
        : _data{}, 
        _family(family)
    {
        switch (family) {
        case AF_INET: {
            _data.v4.sin_family = AF_INET;
            _data.v4.sin_port = htons(port);

            // An omitted address denotes the IPv4 wildcard address.  The
            // value is already zero because _data is value-initialized.
            if (addr.empty())
                return;

            const int result =
                inet_pton(AF_INET, addr.c_str(), &_data.v4.sin_addr);
            if (result == 0)
                throw std::invalid_argument("invalid IPv4 address: " + addr);
            if (result < 0) {
                const int error = errno;
                throw std::system_error(
                    error, std::system_category(), "inet_pton(AF_INET)");
            }
            break;
        }
        case AF_INET6: {
            _data.v6.sin6_family = AF_INET6;
            _data.v6.sin6_port = htons(port);

            // An omitted address denotes the IPv6 wildcard address.  The
            // value is already zero because _data is value-initialized.
            if (addr.empty())
                return;

            const int result =
                inet_pton(AF_INET6, addr.c_str(), &_data.v6.sin6_addr);
            if (result == 0)
                throw std::invalid_argument("invalid IPv6 address: " + addr);
            if (result < 0) {
                const int error = errno;
                throw std::system_error(
                    error, std::system_category(), "inet_pton(AF_INET6)");
            }
            break;
        }
        default:
            throw std::invalid_argument("unsupported address family");
        }
    }

    int family() const
    { 
        return _family; 
    }

    const sockaddr_in & v4() const
    {
        return _data.v4;
    }
    
    sockaddr_in& v4()
    {
        return _data.v4;
    }

    const sockaddr_in6 & v6() const
    {
        return _data.v6;
    }

    sockaddr_in6& v6()
    {
        return _data.v6;
    }

private:
    union {
        sockaddr_storage base;
        sockaddr_in v4;
        sockaddr_in6 v6;
    } _data;
    int _family;
};

}
}
}

#endif
