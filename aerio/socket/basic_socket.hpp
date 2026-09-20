#ifndef AERIO_SOCKET_BASIC_SOCKET_HPP
#define AERIO_SOCKET_BASIC_SOCKET_HPP

#include <aerio/ip/basic_endpoint.hpp>
#include <aerio/socket/detail/socket.hpp>
#include <aerio/core/io_context.hpp>

namespace aerio {
namespace socket {

template <typename ProtocolType>
class basic_socket : public detail::socket {
public:
    explicit basic_socket(const ProtocolType& protocol, aerio::core::io_context& ctx)
        : detail::socket(protocol.family(), protocol.type(), protocol.protocol()),
          _ctx(ctx)
    {}


private:
    aerio::core::io_context &_ctx;
};

}
}


#endif
