#ifndef __OPENCBDC_MML_OPTIONS_H__
#define __OPENCBDC_MML_OPTIONS_H__

#include <util/common/config.hpp>

#include <variant>

namespace cbdc::threepc::mml {
    enum class mode {
        server,
        client,
        not_supported
    };

    struct options {
        std::vector<std::vector<network::endpoint_t>> m_shard_endpoints;

        mode m_active_mode;
        cbdc::logging::log_level m_loglevel;
        size_t m_component_id;
        size_t m_node_id;
    };

    auto read_options(const std::string& config_file)
        ->std::variant<options, std::string>;
    
    auto load_options(const std::string& config_file)
        ->std::variant<options, std::string>;
    
    auto check_options(const options& opts) -> std::optional<std::string>;
}

#endif