#ifndef __OPENCBDC_MML_INTERFACE_H__
#define __OPENCBDC_MML_INTERFACE_H__

#include "util/common/buffer.hpp"
#include "util/common/hashmap.hpp"

#include <optional>
#include <cstdint>
#include <variant>
#include <functional>
#include <unordered_map>

namespace cbdc::threepc::mml {
    using parameter_type = buffer;
    using key_type = buffer;
    using value_type = buffer;

    using return_type = std::unordered_map<key_type, value_type, hashing::const_sip_hash<key_type>>;

    class interface {
        public : 
            virtual ~interface() = default;

            interface() = default;
            interface(const interface&) = delete;
            auto operator=(const interface&) -> interface& = delete;
            interface(const interface&&) = delete;
            auto operator=(const interface&&) -> interface& = delete;

            enum class error_code : uint8_t {
                mml_unreachable,
                function_retrieval,
                function_execution,
                retry,
                internal_error
            };

            using execute_return_type = std::variant<return_type, error_code>;
            using execute_callback_type = std::function<void(execute_return_type)>;

            interface(key_type fn, 
                      parameter_type param, 
                      execute_callback_type result_callback);

            virtual auto exec() -> bool = 0;

            [[nodiscard]] auto get_function() const -> key_type;
            [[nodiscard]] auto get_param() const -> parameter_type;
            [[nodiscard]] auto get_result_callback() const -> execute_callback_type;

        private:
            key_type m_fn;
            parameter_type m_param;
            execute_callback_type m_result_callback;
    };
}
#endif