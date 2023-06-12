#include "3pc/mml/format.hpp"

#include "util/serialization/format.hpp"

namespace cbdc {
    auto operator<<(serializer& ser,
                    const threepc::mml::rpc::execute_request& req)
        -> serializer& {
        return ser << req.m_function << req.m_param << req.m_is_readonly_run;
    }

    auto operator>>(serializer& deser, 
                    threepc::mml::rpc::execute_request& req)
        -> serializer& {
        return deser >> req.m_function >> req.m_param >> req.m_is_readonly_run;
    }

    auto operator<<(serializer& ser, 
                    const threepc::mml::rpc::heartbeat_request& req)
        -> serializer& {
        return ser << req.m_param;
    }

    auto operator>>(serializer& deser,
                    threepc::mml::rpc::heartbeat_request& req) 
        -> serializer& {
        return deser << req.m_param;
    }

    auto operator<<(serializer& ser,
                    const threepc::mml::rpc::heartbeat_response& res)
        -> serializer& {
        return ser << res.m_param;
    }

    auto operator>>(serializer& deser,
                    threepc::mml::rpc::heartbeat_response& res)
        -> serializer& {
        return deser >> res.m_param;
    }
}