#ifndef __OPENCBDC_MML_CLIENT_H__
#define __OPENCBDC_MML_CLIENT_H__

#include "util/common/config.hpp"
#include "util/network/connection_manager.hpp"
#include "util/rpc/tcp_client.hpp"

#include "3pc/mml/interface.hpp"
#include "message.hpp"
#include "3pc/mml/options.hpp"

namespace cbdc::threepc::mml::rpc {
    class client : public interface {
        public :
            client(std::vector<network::endpoint_t> endpoints,
                    std::shared_ptr<logging::log> logger);

            ~client() override = default;
            client() = delete;
            client(const client&) = delete;
            auto operator=(const client&) -> client& = delete;
            client(client&&) = delete;
            auto operator=(const client&&) -> client& = delete;

            auto init(std::optional<bool> error_fatal = std::nullopt) -> bool;

            using execute_result_type = std::optional<execute_return_type>;
        private :
            cbdc::threepc::mml::options m_opts;
            std::shared_ptr<logging::log> m_logger;

            cbdc::rpc::tcp_client<request, response> m_client;
    };
}

#endif