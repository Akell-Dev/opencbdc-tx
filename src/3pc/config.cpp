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
    }
}
