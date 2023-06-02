#include "interface.hpp"

namespace cbdc::threepc::mml {
    interface::interface(key_type fn, 
                        parameter_type param,
                        execute_callback_type result_callback)
        : m_fn(std::move(fn)),
          m_param(std::move(param)),
          m_result_callback(std::move(result_callback)) {}
    
    auto interface::get_function() const -> key_type {
        return m_fn;
    }

    auto interface::get_param() const -> parameter_type {
        return m_param;
    }

    auto interface::get_result_callback() const -> execute_callback_type {
        return m_result_callback;
    }
}