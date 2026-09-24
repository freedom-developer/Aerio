#ifndef AERIO_NET_ENDPOINT_HPP
#define AERIO_NET_ENDPOINT_HPP

#include <aerio/net/protocol.hpp>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <system_error>

namespace aerio {
namespace net {

class endpoint {
public:
    endpoint(const net::protocol_type &protocol, uint16_t port = 0, const std::string &addr = "")
        : _data{}, 
        _protocol(protocol)
    {
        switch (_protocol.family()) {
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

    int family() const noexcept
    { 
        return _protocol.family(); 
    }

    sockaddr* sockaddr_ptr() noexcept
    {
        if (family() == AF_INET)
            return reinterpret_cast<sockaddr*>(&_data.v4);

        return reinterpret_cast<sockaddr*>(&_data.v6);
    }

    const sockaddr* sockaddr_ptr() const noexcept
    {
        if (family() == AF_INET)
            return reinterpret_cast<const sockaddr*>(&_data.v4);

        return reinterpret_cast<const sockaddr*>(&_data.v6);
    }

     size_t size() const noexcept
    {
        if (family() == AF_INET)
            return sizeof(_data.v4);
        return sizeof(_data.v6);
    }

    const net::protocol_type& protocol() const noexcept
    {
        return _protocol;
    }

    const std::string address() const
    {
        const char *ret;
        std::string ipbuf(128, '\0');
        if (family() == AF_INET) {
            ret = ::inet_ntop(AF_INET, &_data.v4.sin_addr, ipbuf.data(), size());
        } else {
            ret = ::inet_ntop(AF_INET6, &_data.v6.sin6_addr, ipbuf.data(), size());
        }
        if (!ret) {
            throw std::system_error(errno, std::system_category(), "inet_ntop");
            return "";
        }

        return ipbuf;
    }

    uint16_t port() const noexcept
    {
        if (family() == AF_INET)
            return ntohs(_data.v4.sin_port);
        return ntohs(_data.v6.sin6_port);
    }

    const std::string address_port() const
    {
        const auto ip = address();
        return ip + ":" + std::to_string(port());
    }

private:
    union {
        sockaddr_storage base;
        sockaddr_in v4;
        sockaddr_in6 v6;
    } _data;

    const net::protocol_type &_protocol;
};

}
}

#endif
