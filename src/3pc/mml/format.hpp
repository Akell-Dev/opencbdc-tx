#ifndef __OPENCBDC_MML_FORMAT_H__
#define __OPENCBDC_MML_FORMAT_H__

#include "3pc/mml/message.hpp"

#include "util/serialization/serializer.hpp"

namespace cbdc {
    auto operator<<(serializer& ser, const threepc::mml::rpc::request& req)
        -> serializer&;

    auto operator>>(serializer& deser, threepc::mml::rpc::request& req)
        -> serializer&;
}

#endif