#ifndef AERIO_SOCKET_BASIC_SOCKET_HPP
#define AERIO_SOCKET_BASIC_SOCKET_HPP

#include <aerio/ip/basic_endpoint.hpp>
#include <aerio/socket/detail/socket.hpp>
#include <aerio/core/io_context.hpp>

namespace aerio {
namespace socket {

namespace detail {
template <typename Protocol, typename = void>
struct has_family : std::false_type {};
template <typename Protocol>
struct has_family <
    Protocol,
    std::void_t<decltype(std::declval<const Protocol&>().family())>
> : std::is_same <
    decltype(std::declval<const Protocol&>().family()), int
> {};

template <typename Protocol, typename = void>
struct has_type : std::false_type {};
template <typename Protocol>
struct has_type <
    Protocol,
    std::void_t<decltype(std::declval<const Protocol&>().type())>
> : std::is_same <
    decltype(std::declval<const Protocol&>().type()), int
> {};


template <typename Protocol, typename = void>
struct has_protocol : std::false_type {};
template <typename Protocol>
struct has_protocol <
    Protocol,
    std::void_t<decltype(std::declval<const Protocol&>().protocol())>
> : std::is_same <
    decltype(std::declval<const Protocol&>().protocol()), int
> {};

}

template <typename ProtocolType>
class basic_socket : public detail::socket {
public:
    static_assert(
        detail::has_family<ProtocolType>::value, 
        "ProtocolType must provide a const family() member returning int"
    );

    static_assert(
        detail::has_type<ProtocolType>::value,
        "ProtocolType must provide a const type() member returning int"
    );

    static_assert(
        detail::has_protocol<ProtocolType>::value,
        "ProtocolType must provide a const protocol() member returning int"
    );

    explicit basic_socket(const ProtocolType& protocol, aerio::core::io_context& ctx)
        : detail::socket(ctx, protocol.family(), protocol.type(), protocol.protocol())
    {}


private:
    
};

}
}


#endif
