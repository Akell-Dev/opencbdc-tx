#include "3pc/config.hpp"

namespace cbdc::threepc {
    auto get_runner_type_key() -> std::string {
        std::stringstream ss;
        ss << runner_prefix << config_separator << runner_postfix;
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

    auto read_agent_options(options& option, const cbdc::config::parser& cfg) 
        -> std::optional<std::string> {
            auto const agent_count_key = get_component_count(agent_prefix);
            auto const agent_count = cfg.get_ulong(agent_count_key).value_or(0);

            for (size_t i{0}; i < agent_count; i++) {
                const auto ith_agent_key = get_ith_component(i, agent_prefix, endpoint_postfix);
                const auto ith_agent_ep = cfg.get_endpoint(ith_agent_key);

                if ( !ith_agent_ep ) {
                    return "No endpoint specified for agent " + std::to_string(i) 
                            + " ( " + ith_agent_key + " )";
                }

                option.m_agent_endpoints.push_back(*ith_agent_ep);
            }

            for (const auto& v : option.m_agent_endpoints) {
                std::cout << v.first << std::endl;
            }

            return std::nullopt;
    }

    auto read_ticket_machine_options(options& option, const cbdc::config::parser& cfg) 
        -> std::optional<std::string> {
            auto const ticket_machine_count_key = get_component_count(ticket_machine_prefix);
            auto const ticket_machine_count = cfg.get_ulong(ticket_machine_count_key).value_or(0);

            for (size_t i{0}; i < ticket_machine_count; i++) {
                const auto ith_ticket_machine_key = get_ith_component(i, ticket_machine_prefix, endpoint_postfix);
                const auto ith_ticket_machine_ep = cfg.get_endpoint(ith_ticket_machine_key);

                if ( !ith_ticket_machine_ep ) {
                    return "No endpoint specified for ticket machine " + std::to_string(i)
                            + " (" + ith_ticket_machine_key + " )";
                }

                option.m_ticket_machine_endpoints.push_back(*ith_ticket_machine_ep);
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
            return "NOT Supported Execution/Transaction Model";
        }

        return std::nullopt;
    }

    // auto read_loadgen_txtype_options(options& option, const cbdc::config::parser& cfg) 
    //     -> std::optional<std::string> {
    //         return std::nullopt;
    // }

    // auto read_loadgen_account_options(options& option, const cbdc::config::parser& cfg)
    //     -> std::optional<std::string> {
    //     return std::nullopt;
    // }

    // auto read_contention_rate_options(options& option, const cbdc::config::parser& cfg)
    //     -> std::optional<std::string> {
    //     return std::nullopt;
    // }

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
            std::cout << "log level not load. setup default logl evel";
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

        {
            if(option.m_runner_type == runner_type::not_supported) {
                return "Three-Phase mode requires [ evm, lua ] Execution/Transaction Model";
            }
        }

        return std::nullopt;
    }
}