#ifndef AERIO_IP_BASIC_ENDPOINT_HPP
#define AERIO_IP_BASIC_ENDPOINT_HPP

#include "detail/endpoint.hpp"

#include <type_traits>
#include <utility>

namespace aerio {
namespace ip {

namespace detail {

/*
 * basic_endpoint stores no ProtocolType object, but it constructs the
 * underlying endpoint from ProtocolType::family().  Keep this check separate
 * from the class so that a missing family() member is reported by the
 * static_assert below instead of producing a long substitution error.
 */
template <typename ProtocolType, typename = void>
struct has_int_family : std::false_type {};

template <typename ProtocolType>
struct has_int_family<
    ProtocolType,
    std::void_t<decltype(std::declval<const ProtocolType&>().family())>>
    : std::is_same<
          decltype(std::declval<const ProtocolType&>().family()), int> {};

} // namespace detail

/* 由模板参数ProtocolType指定端点的family, type, protocol
 * 但basic_endpoint只用到了family
 */
template <typename ProtocolType>
class basic_endpoint : public detail::endpoint {
    static_assert(
        detail::has_int_family<ProtocolType>::value,
        "ProtocolType must provide a const family() member returning int");

public:
    basic_endpoint(const ProtocolType& pt, uint16_t port = 0,  const std::string &addr = "")
        : detail::endpoint(pt.family(), port, addr)
    {}
};

}
}

#endif
