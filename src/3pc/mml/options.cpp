#include "options.hpp"

namespace cbdc::threepc::mml {
    auto read_shard_endpoints([[maybe_unused]]options& opts, const std::string& component_name, const cbdc::config::parser& cfg)
        -> std::optional<std::vector<cbdc::network::endpoint_t>> {
        const auto shard_replica_count_key = component_name + "_count";
    
        const auto shard_replica_count = cfg.get_ulong(shard_replica_count_key).value_or(0);

        auto ret = std::vector<cbdc::network::endpoint_t>();

        if (shard_replica_count == 0 ) {
            return std::nullopt;
        }

        for (size_t i{0}; i < shard_replica_count; i++) {
            const auto shard_replica_key = component_name
                                            + std::to_string(i)
                                            + "_endpoint";
            const auto maybe_shard_replica_ep = cfg.get_endpoint(shard_replica_key);

            if(maybe_shard_replica_ep.has_value()) {
                ret.emplace_back(maybe_shard_replica_ep.value());
            }

            std::cout << maybe_shard_replica_ep.value().first << std::endl;
        }

        return ret;
    }

    auto read_shard_cluster_endpoints([[maybe_unused]]options& opts, const std::string& component_name, const cbdc::config::parser& cfg) 
    -> std::optional<std::vector<std::vector<cbdc::network::endpoint_t>>> {
        const auto shard_master_count_key = component_name + "_count";
        const auto shard_master_count = cfg.get_ulong(shard_master_count_key).value_or(0);

        auto ret = std::vector<std::vector<cbdc::network::endpoint_t>>();

        for ( size_t i{0}; i < shard_master_count; i++) {
            auto node_name = component_name + std::to_string(i);
            auto eps = read_shard_endpoints(opts, node_name, cfg);
            if (!eps.has_value()) {
                return std::nullopt;
            }

            for (const auto& v: eps.value()) {
                std::cout << v.first << std::endl;
            }
            
            ret.emplace_back(eps.value());
        }

        return ret;
    }

    auto read_options(const std::string& config_file)
        -> std::variant<options, std::string> {
        auto opts = options{};
        auto cfg = cbdc::config::parser(config_file);

        opts.m_loglevel = cfg.get_loglevel("loglevel").value_or(cbdc::logging::log_level::warn);

        opts.m_component_id = cfg.get_ulong("component_id").value_or(0);
        opts.m_node_id = cfg.get_ulong("node_id").value_or(0);

        auto ret = read_shard_cluster_endpoints(opts, "shard", cfg);
        if ( !ret.has_value()) {
            return "shard read error";
        }
        opts.m_shard_endpoints = ret.value();

        return opts;
    }

    auto load_options(const std::string& config_file)
        -> std::variant<options, std::string> {
        auto opt = read_options(config_file);
        if(std::holds_alternative<options>(opt)) {
            auto res = check_options(std::get<options>(opt));
            if(res) {
                return *res;
            }
        }
        return opt;
    }

    auto check_options([[maybe_unused]]const options& opts)
    -> std::optional<std::string> {
        if ( opts.m_shard_endpoints.empty()) {
            return "PVM mode requires at least one configure shard master";
        }
        for (size_t i{0}; i < opts.m_shard_endpoints.size(); i++) {
            std::vector<cbdc::network::endpoint_t> master = opts.m_shard_endpoints.at(i);
            if(master.empty()) {
                return "PVM mode requires at least one configured shard "
                       "master "
                     + std::to_string(i) + "at least one configured replica shard";
            }
        }
        return std::nullopt;
    }
}