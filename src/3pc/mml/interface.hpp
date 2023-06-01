#ifndef __OPENCBDC_MML_INTERFACE_H__
#define __OPENCBDC_MML_INTERFACE_H__

#include <optional>

namespace cbdc::threepc::mml::rpc {
    // response struct
    struct execute_response {
        bool m_status{false};
        auto operator==(const execute_response& rhs) const -> bool;
    };

    class interface {
        public : 
            virtual ~interface() = default;

            interface() = default;
            interface(const interface&) = delete;
            auto operator=(const interface&) -> interface& = delete;
            interface(const interface&&) = delete;
            auto operator=(const interface&&) -> interface& = delete;

            virtual auto execute_helthcheck() -> std::optional<execute_response> = 0;
        private:

    };
}
#endif