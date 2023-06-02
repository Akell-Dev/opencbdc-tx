#ifndef __OPENCBDC_MML_IMPL_H__
#define __OPENCBDC_MML_IMPL_H__

#include "3pc/mml/interface.hpp"
// #include "3pc/mml/options.hpp"

// #include "util/common/logging.hpp"
#include "util/common/thread_pool.hpp"

#include <secp256k1.h>

namespace cbdc::threepc::mml {
    class impl {
        public :
            enum class state {
                init,
                cpu_usage_sent,
                cpu_usage_failed,
            };

            impl(
                std::shared_ptr<logging::log> logger,
                options option,
                key_type function,
                parameter_type param,
                interface::execute_callback_type result_callback,
                bool is_readonly_run,
                std::shared_ptr<secp256k1_context> secp,
                std::shared_ptr<thread_pool> t_pool
            );

            ~impl();

            impl(const impl&) = delete;
            auto operator=(const impl&) -> impl& = delete;
            impl(impl&&) = delete;
            auto operator=(impl&&) -> impl& = delete;

            auto exec() -> bool;

        private:
            // std::shared_ptr<logging::log> m_log;
            const options m_cfg;
            key_type m_function;
            parameter_type m_param;
            state m_state{state::init};

            void handle_cpu_usage(interface::execute_return_type& res);

            void do_cpu_usage();
    };
}

#endif