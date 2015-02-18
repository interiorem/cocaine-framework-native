#include "cocaine/framework/util/net.hpp"

namespace cocaine {

namespace framework {

namespace util {

boost::asio::ip::address
address_cast(const asio::ip::address& address) {
    if (address.is_v4()) {
        return boost::asio::ip::address_v4(address.to_v4().to_ulong());
    } else if (address.is_v6()) {
        return boost::asio::ip::address_v6(address.to_v6().to_bytes());
    }

    throw std::invalid_argument("address must be either v4 or v6");
}

asio::ip::address
address_cast(const boost::asio::ip::address& address) {
    if (address.is_v4()) {
        return asio::ip::address_v4(address.to_v4().to_ulong());
    } else if (address.is_v6()) {
        return asio::ip::address_v6(address.to_v6().to_bytes());
    }

    throw std::invalid_argument("address must be either v4 or v6");
}

boost::asio::ip::tcp::endpoint
endpoint_cast(const asio::ip::tcp::endpoint& endpoint) {
    return boost::asio::ip::tcp::endpoint(
        address_cast(endpoint.address()), endpoint.port()
    );
}

asio::ip::tcp::endpoint
endpoint_cast(const boost::asio::ip::tcp::endpoint& endpoint) {
    return asio::ip::tcp::endpoint(
        address_cast(endpoint.address()), endpoint.port()
    );
}

}

}

}