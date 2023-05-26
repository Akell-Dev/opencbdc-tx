#ifndef _OPENCBDC_3PC_SRC_COMMON_CONFIG_H_
#define _OPENCBDC_3PC_SRC_COMMON_CONFIG_H_

#include <vector>
#include <variant>
#include <optional>

#include "util/common/config.hpp"
#include "util/common/logging.hpp"

#include "3pc/util.hpp"

namespace cbdc::threepc {
    namespace defaults {
        static constexpr size_t initial_count{0};
        static constexpr size_t initial_component_id{0};
        static constexpr auto log_level = logging::log_level::trace;
    }

    using endpoints = std::vector<network::endpoint_t>;

    struct options {
        std::vector<network::endpoint_t> m_ticket_machine_endpoints;
        endpoints m_agent_endpoints;
        cbdc::threepc::runner_type m_runner_type;

        size_t m_component_id{0};
        std::optional<size_t> m_node_id; 
        logging::log_level m_loglevel;
    };

    static constexpr auto endpoint_postfix = "endpoint";
    static constexpr auto loglevel_postfix = "loglevel";
    static constexpr auto shard_count_prefix = "shard_count";
    static constexpr auto shard_prefix = "shard";
    static constexpr auto ticket_machine_prefix = "ticket_machine";
    static constexpr auto loadgen_prefix = "loadgen";
    static constexpr auto agent_prefix = "agent";
    static constexpr auto runner_prefix = "runner";
    static constexpr auto config_separator = "_";
    static constexpr auto count_postfix = "count";
    static constexpr auto runner_postfix = "type";
    static constexpr auto txtype_postfix = "txtype";
    static constexpr auto account_postfix = "account";

    static constexpr auto component_id_prefix = "component_id";
    static constexpr auto node_id_prefix = "node_id";
    static constexpr auto contention_rate_prefix = "contention_rate";

    auto read_agent_options(options& option, const cbdc::config::parser& cfg) -> std::optional<std::string>;
    auto read_ticket_machine_options(options& option, const cbdc::config::parser& cfg) -> std::optional<std::string>;
    auto read_runner_options(options& option, const cbdc::config::parser& cfg) -> std::optional<std::string>;
    //auto read_shard_cluster_options(config& option, const parser& cfg) -> std::optional<std::string>;
    //auto read_shard_options(config& option, const parser& cfg) -> std::optional<std::string>;
    auto read_log_level_options(options& option, const cbdc::config::parser& cfg) -> std::optional<std::string>;
    auto read_loadgen_txtype_options(options& option, const cbdc::config::parser& cfg) -> std::optional<std::string>;
    auto read_loadgen_account_options(options& option, const cbdc::config::parser& cfg) -> std::optional<std::string>;
    auto read_contention_rate_options(options& option, const cbdc::config::parser& cfg) -> std::optional<std::string>;

    auto load_options(const std::string& config_file) -> std::variant<options, std::string>;
    auto read_options(const std::string& config_file) -> std::variant<options, std::string>;

    auto check_options(const options& option) -> std::optional<std::string>;
}

#endif