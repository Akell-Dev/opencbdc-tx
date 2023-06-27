// Copyright (c) 2021 MIT Digital Currency Initiative,
//                    Federal Reserve Bank of Boston
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "controller.hpp"
#include "util.hpp"
#include "util/common/logging.hpp"
#include "util/rpc/format.hpp"
#include "util/rpc/tcp_server.hpp"
#include "util/serialization/format.hpp"

#include "3pc/config.hpp"

#include <csignal>

auto main(int argc, char** argv) -> int {
    auto log
        = std::make_shared<cbdc::logging::log>(cbdc::logging::log_level::trace);
    
    auto args = cbdc::config::get_args(argc, argv);
    if ( args.size() < 5) {
        std::cerr << "Usage: " << args[0] << " <component id> <node id>"
                  << " <configure file> <log file>" << std::endl;
        return -1;
    }

    auto maybe_option = cbdc::threepc::load_options(args[3]);
    if ( std::holds_alternative<std::string>(maybe_option)) {
        std::cerr << "Error loading config file : " << args[3] << "\n"
                  << std::get<std::string>(maybe_option) << std::endl;
    }

    auto option = std::get<cbdc::threepc::options>(maybe_option);

    auto cfg = cbdc::threepc::convert(option, args[1], args[2]);
    if (!cfg.has_value()) {
        std::cout << "not converted cfg" << std::endl;
        return -1;
    }
    
    log->set_loglevel(cfg->m_loglevel);
    log->set_logfile_name(args[4]);

    if(cfg->m_component_id >= cfg->m_ticket_machine_endpoints.size()) {
        log->error("No endpoint specified for ticket machine");
        return 1;
    }

    auto raft_endpoints = std::vector<cbdc::network::endpoint_t>();
    for(auto& e : cfg->m_ticket_machine_endpoints) {
        auto new_ep = e;
        new_ep.second++;
        raft_endpoints.push_back(new_ep);
    }

    auto raft_server = cbdc::threepc::ticket_machine::controller(
        cfg->m_component_id,
        cfg->m_ticket_machine_endpoints[cfg->m_component_id],
        raft_endpoints,
        log);
    if(!raft_server.init()) {
        log->error("Failed to start raft server");
        return 1;
    }

    static auto running = std::atomic_bool{true};

    std::signal(SIGINT, [](int /* signal */) {
        running = false;
    });

    log->info("Ticket machine running");

    while(running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    log->info("Shutting down...");

    return 0;
}
