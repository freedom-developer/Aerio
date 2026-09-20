#ifndef AERIO_SOCKET_LISTENER_HPP
#define AERIO_SOCKET_LISTENER_HPP

#include <aerio/ip/basic_endpoint.hpp>
#include <aerio/socket/basic_socket.hpp>
#include <asm-generic/errno.h>
#include <system_error>

namespace aerio {
namespace socket {

template <typename ProtocolType>
class listener : public basic_socket<ProtocolType>{
public:
    using endpoint_type = aerio::ip::basic_endpoint<ProtocolType>;

    listener(const ProtocolType& protocol,
             aerio::core::io_context& ctx,
             const endpoint_type& ep)
        : basic_socket<ProtocolType>(protocol, ctx)
    {
        auto ret = this->bind(ep);
        if (ret < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "bind");
        }

        ret = this->listen();
        if (ret < 0) {
            const auto err = errno;
            throw std::system_error(err, std::system_category(), "listen");
        }
    }

private:



};

}
}

#endif
