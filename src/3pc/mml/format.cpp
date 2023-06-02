#include "3pc/mml/format.hpp"

#include "util/serialization/format.hpp"

namespace cbdc {
    auto operator<<(serializer& ser, [[maybe_unused]]const threepc::mml::rpc::request& req)
        -> serializer& {
        return ser;
    }

    auto operator>>(serializer& deser, [[maybe_unused]] threepc::mml::rpc::request& req)
        -> serializer& {
        return deser;
    }
}