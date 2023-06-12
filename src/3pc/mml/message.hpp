#ifndef __OPENCBDC_MML_MESSAGE_H__
#define __OPENCBDC_MML_MESSAGE_H__

#include "3pc/mml/interface.hpp"

#include <variant>

namespace cbdc::threepc::mml::rpc {
    /// MML contract execution RPC request message.
    struct execute_request {
        /// Key of function bytecode :  m_function
        key_type m_function;
        /// Function call parameter : m_param
        parameter_type m_param;
        /// Whether the request should skip writing state changes.
        bool m_is_readonly_run{false};
    };

    struct heartbeat_request {
        parameter_type m_param;
    };

    struct heartbeat_response {
        uint8_t m_param;
    };

    struct execute_response {
        uint8_t _result;
        std::string _result_str;
    };

    /// MML RPC request type.
    using request = std::variant<execute_request, heartbeat_request>;
    /// MML RPC response type.
    using response = std::variant<execute_response, heartbeat_response>;
}
#endif