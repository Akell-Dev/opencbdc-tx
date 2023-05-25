#include "3pc/config.hpp"

namespace cbdc::threepc {
    auto get_runner_type_key() -> std::string {
        std::stringstream ss;
        ss << runner_prefix << config_separator << runner_postfix;
        return ss.str();
    }

    auto get_component_prefix(std::stringstream& ss, size_t id, std::string key_name) {
        ss << key_name << id << config_separator;
    }

    auto get_component_count(std::string key) -> std::string {
        std::stringstream ss;
        ss << key << config_separator << count_postfix;
        return ss.str();
    }

    // auto get_ticket_machine_endpoint() -> std::string {

    // }

    auto read_runner_options(options& option, const cbdc::config::parser& cfg) 
        -> std::optional<std::string> {
        auto runner_key = get_runner_type_key();
        auto runner = cfg.get_string(runner_key);

        if ( runner == "evm") {
            option.m_runner_type = runner_type::evm;
        } else if (runner == "lua") {
            option.m_runner_type = runner_type::lua;
        } else {
            return "NOT Supported Execution/Transaction Model";
        }

        return std::nullopt;
    }

    auto read_options(const std::string& config_file) 
        -> std::variant<options, std::string> {
        auto opts = options{};
        auto cfg = cbdc::config::parser(config_file);

        auto component_id = cfg.get_ulong("component_id");
        if (!component_id.has_value()) {
            return "Component ID is Required!";
        }
        opts.m_component_id = component_id.value();

        auto node_id = cfg.get_ulong("node_id");
        opts.m_node_id = node_id;
        
        auto err = read_runner_options(opts, cfg);
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
            return "Three-Phase mode requires at least on configured ticket machine";
        }

        {
            if(option.m_runner_type == runner_type::not_supported) {
                return "Three-Phase mode requires [ evm, lua ] Execution/Transaction Model";
            }
        }

        return std::nullopt;
    }
}