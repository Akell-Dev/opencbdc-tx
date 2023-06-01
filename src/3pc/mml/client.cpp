#include "client.hpp"

namespace cbdc::threepc::mml::rpc {
    client::client(std::vector<network::endpoint_t> endpoints,
                    std::shared_ptr<logging::log> logger)
        : m_logger(std::move(logger)),
          m_client(std::move(endpoints)) {}

    auto client::init(std::optional<bool> error_fatal) -> bool {
        if (!m_client.init(error_fatal)) {
            m_logger->error("Failed to initialize mml RPC Client");
            return false;
        }

        return true;
    }

} 
