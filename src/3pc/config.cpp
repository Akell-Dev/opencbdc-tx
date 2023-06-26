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
            auto const maybe_component_count = cfg.get_ulong(component_count_key);

            auto component_count = maybe_component_count.value();

            for(size_t i{0}; i < component_count; i++) {
                const auto ith_component_key = get_ith_component(i, component_prefix, endpoint_postfix);
                const auto ith_component_ep = cfg.get_endpoint(ith_component_key);

                if (!ith_component_ep) {
                    return std::nullopt;
                }

                ret.push_back(*ith_component_ep);
            }

            return ret;
    }

    auto read_agent_options(options& option, const cbdc::config::parser& cfg)
        -> std::optional<std::string> {
            auto maybe_agent_endpoints = read_endpoints(cfg, agent_prefix);

            if(!maybe_agent_endpoints.has_value()) {
                return "No endpoints specified for agent";
            }

            auto agent_endpoints = maybe_agent_endpoints.value();

            if(agent_endpoints.empty()) {
                return "Agent endpoints empty";
            }

            option.m_agent_endpoints = agent_endpoints;

            return std::nullopt;
    }

        auto read_ticket_machine_options(options& option, const cbdc::config::parser& cfg)
            -> std::optional<std::string> {
                auto maybe_ticket_machine_endpoints = read_endpoints(cfg, ticket_machine_prefix);
                if ( !maybe_ticket_machine_endpoints.has_value()) {
                    return "No endpoints specifed for ticket machine";
                }

                auto ticket_machine_endpoints = maybe_ticket_machine_endpoints.value();

                if ( ticket_machine_endpoints.empty()) {
                    return "No ticket machine endpoints empty";
                }

                option.m_ticket_machine_endpoints = ticket_machine_endpoints;

                return std::nullopt;
        }

        auto read_shard_options(options& option, size_t cluster_id,
                                const cbdc::config::parser& cfg) 
            -> std::optional<std::string> {
                auto component_ith_count_key = get_ith_component(cluster_id, shard_prefix, count_postfix);
                auto component_ith_key = get_ith_component(cluster_id, shard_prefix, "");

                auto maybe_shard_ith_count = cfg.get_ulong(component_ith_count_key);
                if ( !maybe_shard_ith_count.has_value()) {
                    return "Three-Phase mode requires at least on configured shard replicated";
                }

                auto shard_ith_count = maybe_shard_ith_count.value();
                auto shard_endpoints = endpoints();

                for(size_t i{0}; i < shard_ith_count; i++) {
                    auto component_key = get_replica_component(cluster_id, i, shard_prefix, endpoint_postfix);
                    auto maybe_shard_endpoint = cfg.get_endpoint(component_key);

                    if (!maybe_shard_endpoint.has_value()) {
                        return component_key + " empty";
                    }

                    auto shard_endpoint = maybe_shard_endpoint.value();

                    shard_endpoints.push_back(shard_endpoint);
                }
            
            option.m_shard_endpoints.push_back(shard_endpoints);

            return std::nullopt;
        }

        auto read_shard_cluster_options(options& option,
                                        const cbdc::config::parser& cfg)
            -> std::optional<std::string> {
            auto maybe_shard_cluster_count = cfg.get_ulong(shard_count_prefix);

            if(!maybe_shard_cluster_count.has_value()) {
                    return "Three-Phase mode requires at least on configured "
                           "shard cluster";
            }

            auto shard_cluster_count = maybe_shard_cluster_count.value();
            for(size_t i{0}; i < shard_cluster_count; i++) {
                    read_shard_options(option, i, cfg);
            }

            return std::nullopt;
        }

        auto read_runner_options(options& option,
                                 const cbdc::config::parser& cfg)
            -> std::optional<std::string> {
            auto runner_key = get_runner_type_key();
            auto runner = cfg.get_string(runner_key);

            if ( !runner.has_value()) {
                return "Three-Phase mode requires at least on configured Execution/Transaction Model";
            }

            if ( runner.value() == "evm") {
                option.m_runner_type = runner_type::evm;
            } else if ( runner.value() == "lua") {
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
            auto maybe_loadgen_txtype = cfg.get_string(loadgen_txtype_key);

            if( !maybe_loadgen_txtype.has_value()) {
                return "Loadgen Tx Type Requires";
            }

            auto loadgen_txtype = maybe_loadgen_txtype.value();
            if ( loadgen_txtype == "transfer" ) {
                option.m_load_type = load_type::transfer;
            } else if ( loadgen_txtype == "erc20" ) {
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
                auto maybe_loadgen_accounts = cfg.get_ulong(loadgen_accounts_key);

                if( !maybe_loadgen_accounts.has_value() ) {
                    return "Accounts is not defined";
                }

                option.m_loadgen_accounts = maybe_loadgen_accounts.value();

                return std::nullopt;
        }

        auto read_contention_rate_options(options& option,
                                          const cbdc::config::parser& cfg)
            -> std::optional<std::string> {
                auto maybe_contention_rate
                    = cfg.get_decimal(contention_rate_prefix);

                if( !maybe_contention_rate.has_value() ) {
                    option.m_contention_rate = defaults::initial_contention_rate;
                } else {
                    option.m_contention_rate = maybe_contention_rate.has_value();
                }

                return std::nullopt;
        }

        auto read_options(const std::string& config_file) 
            -> std::variant<options, std::string> {
                auto opts = options{};
                auto cfg = cbdc::config::parser(config_file);
                
                // auto maybe_component_id = cfg.get_ulong(component_id_prefix);
                // if(!maybe_component_id.has_value()) {
                //     return "Component ID required";
                // }

                // opts.m_component_id = maybe_component_id.value();

                // auto maybe_node_id = cfg.get_ulong(node_id_prefix);
                // opts.m_node_id = maybe_node_id;

                auto maybe_log_level = cfg.get_string(loglevel_postfix);
                if ( maybe_log_level.has_value()) {
                    auto log_level = logging::parse_loglevel(maybe_log_level.value());
                    if ( log_level.has_value()) {
                        opts.m_loglevel = log_level.value();
                    } else {
                        opts.m_loglevel = defaults::initial_log_level;
                    }

                } else {
                    std::cout << "log level not load. setup default loglevel" << std::endl;
                    opts.m_loglevel = defaults::initial_log_level;
                }

                auto err = read_ticket_machine_options(opts, cfg);

                if ( err.has_value() ) {
                    return err.value();
                }

                err = read_agent_options(opts, cfg);
                if ( err.has_value() ) {
                    return err.value();
                }

                err = read_shard_cluster_options(opts, cfg);
                if ( err.has_value() ) {
                    return err.value();
                }

                err = read_runner_options(opts, cfg);
                if ( err.has_value() ) {
                    return err.value();
                }

                err = read_loadgen_txtype_options(opts, cfg);
                if ( err.has_value() ) {
                    return err.value();
                }

                err = read_loadgen_account_options(opts, cfg);
                if ( err.has_value() ) {
                    return err.value();
                }
                
                err = read_contention_rate_options(opts, cfg);
                if ( err.has_value() ) {
                    return err.value();
                }

                return opts;
        }

        auto check_options(const options& option) 
            -> std::optional<std::string> {
                // check ticket machine 
                if ( option.m_ticket_machine_endpoints.empty()) {
                    return "Three-Phase mode requires at least on configured ticket machine";
                }

                /// check agent 
                if ( option.m_agent_endpoints.empty())  {
                    return "Three-Phase mode requires at least on configured agent";
                }

                /// check runner type
                if ( option.m_runner_type >= runner_type::not_supported) {
                    return "Three-Phase mode require [ evm, lua ] Execution/Transaction Model";
                }

                /// check transfer mode
                if ( option.m_load_type >= load_type::not_supported) {
                    return "Three-Phase mode require [ transfer, erc20 ] Generator Model";
                }

                /// check accounts
                if ( option.m_loadgen_accounts <= 0) {
                    return "Three-Phase mode account";
                }

                /// check contention rate

                return std::nullopt;
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

        auto convert(const options option , std::string& component_id, std::string& node_id) 
            -> std::optional<cbdc::threepc::config> {
                auto config = cbdc::threepc::config{};

                auto maybe_component_id = cbdc::threepc::fromString<int>(component_id);
                size_t real_component_id = 0;
                if ( maybe_component_id.has_value()) {
                    real_component_id = maybe_component_id.value();
                }

                config.m_component_id = real_component_id;
                config.m_node_id = cbdc::threepc::fromString<size_t>(node_id);
                config.m_loglevel = option.m_loglevel;
                config.m_runner_type = option.m_runner_type;
                config.m_contention_rate = option.m_contention_rate;
                config.m_loadgen_accounts = option.m_loadgen_accounts;
                config.m_load_type = option.m_load_type;
                
                for (auto v : option.m_ticket_machine_endpoints) {
                    config.m_ticket_machine_endpoints.push_back(v);
                }

                for (auto v : option.m_agent_endpoints) {
                    config.m_agent_endpoints.push_back(v);
                }

                config.m_shard_endpoints.assign(option.m_shard_endpoints.begin(), option.m_shard_endpoints.end());
                copy(option.m_shard_endpoints.begin(), option.m_shard_endpoints.end(), config.m_shard_endpoints.begin());

                return config;
        }
}
