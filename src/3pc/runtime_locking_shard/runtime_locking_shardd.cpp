// Copyright (c) 2021 MIT Digital Currency Initiative,
//                    Federal Reserve Bank of Boston
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "controller.hpp"
#include "util.hpp"
#include "util/common/logging.hpp"

#include "3pc/config.hpp"

#include <csignal>

auto main(int argc, char** argv) -> int {
    auto log
        = std::make_shared<cbdc::logging::log>(cbdc::logging::log_level::warn);

    auto args = cbdc::config::get_args(argc, argv);
    if ( args.size() < 5 ) {
        std::cerr << "Usage: " << args[0] << " <component id> <node id> "
                  << " <configure file> <log file>" << std::endl;
        return -1;
    }

    auto maybe_option = cbdc::threepc::load_options(args[3]);
    if(std::holds_alternative<std::string>(maybe_option)) {
        std::cerr << "Error loading config file : " << args[3] << "\n"
                  << std::get<std::string>(maybe_option) << std::endl;

        return -1;
    }

    auto option = std::get<cbdc::threepc::options>(maybe_option);

    auto cfg = cbdc::threepc::convert(option, args[1], args[2]);

    if (!cfg.has_value()) {
        std::cout << "not converted cfg" << std::endl;
    }

    log->set_loglevel(cfg->m_loglevel);
    log->set_logfile_name(args[4]);

    if(cfg->m_shard_endpoints.size() <= cfg->m_component_id) {
        log->error("No endpoint for component id");
        return 1;
    }

    if(cfg->m_shard_endpoints[cfg->m_component_id].size() <= *cfg->m_node_id) {
        log->error("No endpoint for node id");
        return 1;
    }

    auto raft_endpoints = std::vector<cbdc::network::endpoint_t>();
    for(auto& e : cfg->m_shard_endpoints[cfg->m_component_id]) {
        auto new_ep = e;
        new_ep.second++;
        raft_endpoints.push_back(new_ep);
    }

    auto controller = cbdc::threepc::runtime_locking_shard::controller(
        cfg->m_component_id,
        *cfg->m_node_id,
        cfg->m_shard_endpoints[cfg->m_component_id][*cfg->m_node_id],
        raft_endpoints,
        log);
    if(!controller.init()) {
        log->error("Failed to start raft server");
        return 1;
    }

    static auto running = std::atomic_bool{true};

    std::signal(SIGINT, [](int /* signal */) {
        running = false;
    });

    log->info("Shard running");

    while(running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    log->info("Shutting down...");
    return 0;
}
