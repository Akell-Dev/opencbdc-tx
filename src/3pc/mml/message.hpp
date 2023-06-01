#ifndef __OPENCBDC_MML_MESSAGE_H__
#define __OPENCBDC_MML_MESSAGE_H__

#include "util/common/buffer.hpp"
#include "interface.hpp"

#include <variant>

namespace cbdc::threepc::mml::rpc {
    using key_type = buffer;
    using parameter_type = buffer;

    enum class error_code : uint8_t {
        internal_error
    };
    
    using exec_return_type = std::variant<std::string, error_code>;

    /// MML contract execution RPC request message.
    struct execute_request {
        /// Key of function bytecode :  m_function
        key_type m_function;
        /// Function call parameter : m_param
        parameter_type m_param;
        /// Whhether the request should skip writing state changes.
        bool m_is_readonly_run{false};
    };

    /// MML RPC request type.
    using request = std::variant<execute_request>;
    /// MML RPC response type.
    using response = std::variant<execute_response>;
}
#endif