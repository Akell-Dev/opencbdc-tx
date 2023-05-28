#include "3pc/config.hpp"

namespace cbdc::threepc {
    auto get_runner_type_key() -> std::string {
        std::stringstream ss;
        ss << runner_prefix << config_separator << runner_postfix;
        return ss.str();
    }

    auto get_loadgen_txtype_key() -> std::string {
        std::stringstream ss;
        ss << loadgen_prefix << config_separator << txtype_postfix;
        return ss.str();
    }

    auto get_loadgen_accounts_key() -> std::string {
        std::stringstream ss;
        ss << loadgen_prefix << config_separator << account_postfix;
        return ss.str();
    }

    auto get_component_prefix(std::stringstream& ss, size_t id, std::string key) {
        ss << key << id << config_separator;
    }

    auto get_component_postfix(std::stringstream& ss, std::string key) {
        ss << key;
    }

    auto get_component_count(std::string key) -> std::string {
        std::stringstream ss;
        ss << key << config_separator << count_postfix;
        return ss.str();
    }

    /// i'th component 
    auto get_ith_component(size_t id, std::string prefix, std::string postfix) -> std::string {
        std::stringstream ss;
        get_component_prefix(ss, id, prefix);
        get_component_postfix(ss, postfix);
        return ss.str();
    }

    auto get_replica_component(size_t cluster_id, size_t replica_id, std::string prefix, std::string postfix) 
        -> std::string {
            std::stringstream ss;
            ss << prefix << cluster_id << replica_id << config_separator << postfix;
            return ss.str();
    }

    auto read_endpoints(cbdc::config::parser cfg, std::string component_prefix)
        -> std::optional<endpoints> {
        auto ret = endpoints();

        auto const component_count_key = get_component_count(component_prefix);
        auto const component_count = cfg.get_ulong(component_count_key);

        for(size_t i{0}; i < component_count; i++) {
            const auto ith_component_key
                = get_ith_component(i, component_prefix, endpoint_postfix);
            const auto ith_component_ep = cfg.get_endpoint(ith_component_key);

            if(!ith_component_ep) {
                return std::nullopt;
            }

            ret.push_back(*ith_component_ep);
        }

        return ret;
    }

    auto read_agent_options(options& option, const cbdc::config::parser& cfg) 
        -> std::optional<std::string> {

            auto maybe_agent_endpoints = read_endpoints(cfg, agent_prefix);
            if (!maybe_agent_endpoints.has_value()) {
                return "No endpoints specified for agent";
            }

            auto agent_endpoints = maybe_agent_endpoints.value();
            
            if ( agent_endpoints.empty()) {
                // TODO : Error Message Re-defined
                return "agent endpoints empty";
            }

            option.m_agent_endpoints = agent_endpoints;

            return std::nullopt;
    }

    auto read_ticket_machine_options(options& option,
                                     const cbdc::config::parser& cfg)
        -> std::optional<std::string> {
            auto maybe_ticket_machine_endpoints = read_endpoints(cfg, ticket_machine_prefix);
            if ( !maybe_ticket_machine_endpoints.has_value()) {
                return "No endpoints specifed for ticket machine";
            }

            auto ticket_machine_endpoints = maybe_ticket_machine_endpoints.value();

            if (ticket_machine_endpoints.empty()) {
                // TODO : Error Message Re-defined
                return "ticket machine endpoints empty";
            }

            option.m_ticket_machine_endpoints = ticket_machine_endpoints;

        return std::nullopt;
    }

    auto read_shard_options([[maybe_unused]]options& option,
                            size_t cluster_id,
                            const cbdc::config::parser& cfg)
        -> std::optional<std::string> {
        auto component_ith_count_key = get_ith_component(cluster_id, shard_prefix, count_postfix);
        auto component_ith_key = get_ith_component(cluster_id, shard_prefix, "");

        auto maybe_shard_ith_count = cfg.get_ulong(component_ith_count_key);

        if (!maybe_shard_ith_count.has_value()) {
            return "Three-Phase mode requires at least on configured shard replicated";
        }

        auto shard_ith_count = maybe_shard_ith_count.value();
        [[maybe_unused]]auto shard_endpoints = endpoints();
        
        for(size_t i{0}; i < shard_ith_count; i++) {
            auto component_key = get_replica_component(cluster_id, i, shard_prefix, "endpoint");
            auto maybe_shard_endpoint = cfg.get_endpoint(component_key);

            if (!maybe_shard_endpoint.has_value()) {
                // TODO : Error Message Re-defined;
                return component_key + " empty";
            }
            auto shard_endpoint = maybe_shard_endpoint.value();

            shard_endpoints.push_back(shard_endpoint);
        }

        option.m_shard_endpoints.push_back(shard_endpoints);

        return std::nullopt;
    }

    auto read_shard_cluster_options(options& option, const cbdc::config::parser& cfg)
        -> std::optional<std::string> {
            auto maybe_shard_cluster_count = cfg.get_ulong(shard_count_prefix);
            if ( !maybe_shard_cluster_count.has_value()) {
                return "Three-Phase mode requires at least on configured shard cluster";
            }

            auto shard_cluster_count = maybe_shard_cluster_count.value();
            for (size_t i{0}; i < shard_cluster_count; i++) {
                read_shard_options(option, i, cfg);
            }

            for ( size_t i{0}; i < option.m_shard_endpoints.size(); i++) {
                std::cout << std::to_string(i) << std::endl;
                endpoints ep = option.m_shard_endpoints.at(i);
                for ( size_t j{0}; j < ep.size(); j++) {
                    std::cout << std::to_string(j) << std::endl;
                    std::cout << ep.at(j).first << ":" << std::to_string(ep.at(j).second) << std::endl;
                }
            }
            return std::nullopt;
        }

    auto read_runner_options(options& option, const cbdc::config::parser& cfg) 
        -> std::optional<std::string> {
        auto runner_key = get_runner_type_key();
        auto runner = cfg.get_string(runner_key);

        if ( !runner.has_value()) {
            return "Three-Phase mode requires at least on configured Execution/Transaction Model";
        }

        if ( runner.value() == "evm") {
            option.m_runner_type = runner_type::evm;
        } else if (runner.value() == "lua") {
            option.m_runner_type = runner_type::lua;
        } else {
            option.m_runner_type = runner_type::not_supported;
        }

        return std::nullopt;
    }

    auto read_loadgen_txtype_options(options& option,
                                     const cbdc::config::parser& cfg)
        -> std::optional<std::string> {
        auto loadgen_txtype_key = get_loadgen_txtype_key();
        auto loadgen_txtype = cfg.get_string(loadgen_txtype_key);

        if(!loadgen_txtype.has_value()) {
            // TODO : Error Message Re-defined
            return "Loadgen TxType requires";
        }

        if(loadgen_txtype.value() == "transfer") {
            option.m_load_type = load_type::transfer;
        } else if(loadgen_txtype.value() == "erc20") {
            option.m_load_type = load_type::erc20;
        } else {
            option.m_load_type = load_type::not_supported;
        }

        return std::nullopt;
    }

    auto read_loadgen_account_options(options& option,
                                      const cbdc::config::parser& cfg)
        -> std::optional<std::string> {
            auto loadgen_accounts_key = get_loadgen_accounts_key();
            auto loadgen_accounts = cfg.get_ulong(loadgen_accounts_key);

            if(!loadgen_accounts.has_value()) {
                // TODO : Error Message Re-defined
                return "Accounts is not Defined";
            }

            option.m_loadgen_accounts = loadgen_accounts.value();

            return std::nullopt;
    }

    auto read_contention_rate_options(options& option,
                                      const cbdc::config::parser& cfg)
        -> std::optional<std::string> {
        auto contention_rate = cfg.get_decimal(contention_rate_prefix);

        if(contention_rate.has_value()) {
                // TODO : Error Message Re-defined
                return "Contention Rate is not Defined";
        }

        option.m_contention_rate = contention_rate.has_value();
        return std::nullopt;
    }

    auto read_options(const std::string& config_file) 
        -> std::variant<options, std::string> {
        auto opts = options{};
        auto cfg = cbdc::config::parser(config_file);

        auto component_id = cfg.get_ulong(component_id_prefix);
        if (!component_id.has_value()) {
            return "Component ID is Required!";
        }
        opts.m_component_id = component_id.value();

        auto node_id = cfg.get_ulong(node_id_prefix);
        opts.m_node_id = node_id;

        auto log_level = cfg.get_string(loglevel_postfix);
        if ( log_level.has_value()) {
            auto maybe_loglevel = logging::parse_loglevel(log_level.value());
            if ( maybe_loglevel.has_value()) {
                opts.m_loglevel = maybe_loglevel.value();
            }
        } else {
            std::cout << "log level not load. setup default loglevel";
            opts.m_loglevel = defaults::log_level;
        }

        auto err = read_ticket_machine_options(opts, cfg);
        if ( err.has_value() ) {
            return err.value();
        }

        err = read_agent_options(opts, cfg);
        if ( err.has_value()) {
            return err.value();
        }

        err = read_shard_cluster_options(opts, cfg);
        if ( err.has_value()) {
            return err.value();
        }

        err = read_runner_options(opts, cfg);
        if( err.has_value() ) {
            return err.value();
        }

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

    auto check_options(const options& option) 
        -> std::optional<std::string> {
        if (option.m_ticket_machine_endpoints.empty()) {
            return "Three-Phase mode requires at least on configured ticket machine";
        }

        if (option.m_agent_endpoints.empty()) {
            return "Three-Phase mode requires at least on configured agent";
        }

        /// check runner type
        {
            if(option.m_runner_type == runner_type::not_supported) {
                return "Three-Phase mode requires [ evm, lua ] Execution/Transaction Model";
            }
        }

        // TODO : check transfer mode

        // TODO : check accounts

        // TODO : check contention rate

        return std::nullopt;
    }
}