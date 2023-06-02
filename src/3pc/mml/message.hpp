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

    /// MML RPC request type.
    using request = execute_request;
    /// MML RPC response type.
    using response = interface::execute_return_type;
}
#endif