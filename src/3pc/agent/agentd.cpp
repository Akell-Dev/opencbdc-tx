// Copyright (c) 2021 MIT Digital Currency Initiative,
//                    Federal Reserve Bank of Boston
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "broker/impl.hpp"
#include "crypto/sha256.h"
#include "directory/impl.hpp"
#include "format.hpp"
#include "impl.hpp"
#include "runners/evm/format.hpp"
#include "runners/evm/http_server.hpp"
#include "runners/evm/math.hpp"
#include "runners/evm/messages.hpp"
#include "runners/evm/util.hpp"
#include "runners/lua/server.hpp"
#include "runtime_locking_shard/client.hpp"
#include "ticket_machine/client.hpp"
#include "util.hpp"
#include "util/common/logging.hpp"
#include "util/rpc/format.hpp"
#include "util/rpc/tcp_server.hpp"
#include "util/serialization/format.hpp"

#include "3pc/config.hpp"

#include <csignal>
#include <memory>

auto main(int argc, char** argv) -> int {
    auto log = std::make_shared<cbdc::logging::log>(
        cbdc::logging::log_level::trace);

    auto sha2_impl = SHA256AutoDetect();
    log->info("using sha2: ", sha2_impl);

    auto args = cbdc::config::get_args(argc, argv);
    if(args.size() < 5) {
        std::cerr << "Usage: " << args[0] << " <component id> <node id>"
                  << "<configure file> <log file>" << std::endl;
        return -1;
    }

    auto maybe_option = cbdc::threepc::load_options(args[3]);
    if(std::holds_alternative<std::string>(maybe_option)) {
        std::cerr << "Error loading config file: test.cfg\n"
                  << std::get<std::string>(maybe_option) << std::endl;
        return -1;
    }

    auto option = std::get<cbdc::threepc::options>(maybe_option);

    auto cfg
        = cbdc::threepc::convert(option, args[1], args[2]);

    if(!cfg.has_value()) {
        std::cout << "not converted cfg" << std::endl;
        return -1;
    }

    std::cout << cfg->m_component_id << std::endl;
    std::cout << cfg->m_node_id.value() << std::endl;

    // auto cfg = cbdc::threepc::read_config(argc, argv);
    // if(!cfg.has_value()) {
    //     log->error("Error parsing options");
    //     return 1;
    // }

    /// set Log Settings
    log->set_loglevel(cfg->m_loglevel);
    log->set_logfile_name(args[4]);

    if(cfg->m_agent_endpoints.size() <= cfg->m_component_id) {
        log->error("No endpoint for component id");
        return 1;
    }

    log->info("Connecting to shards...");

    auto shards = std::vector<
        std::shared_ptr<cbdc::threepc::runtime_locking_shard::interface>>();
    for(const auto& shard_ep : cfg->m_shard_endpoints) {
        auto client = std::make_shared<
            cbdc::threepc::runtime_locking_shard::rpc::client>(
            std::vector<cbdc::network::endpoint_t>{shard_ep});
        if(!client->init()) {
            log->error("Error connecting to shard");
            return 1;
        }
        shards.emplace_back(client);
    }

    log->info("Connected to shards, connecting to ticketer...");

    auto ticketer
        = std::make_shared<cbdc::threepc::ticket_machine::rpc::client>(
            std::vector<cbdc::network::endpoint_t>{
                cfg->m_ticket_machine_endpoints});
    if(!ticketer->init()) {
        log->error("Error connecting to ticket machine");
        return 1;
    }

    auto directory
        = std::make_shared<cbdc::threepc::directory::impl>(shards.size());
    auto broker
        = std::make_shared<cbdc::threepc::broker::impl>(cfg->m_component_id,
                                                        shards,
                                                        ticketer,
                                                        directory,
                                                        log);

    log->info("Requesting broker recovery...");

    auto recover_success = std::promise<bool>();
    auto recover_fut = recover_success.get_future();
    auto success = broker->recover(
        [&](cbdc::threepc::broker::interface::recover_return_type res) {
            recover_success.set_value(!res.has_value());
        });
    if(!success) {
        log->error("Error requesting broker recovery");
        return 1;
    }

    constexpr auto recover_delay = std::chrono::seconds(60);
    auto wait_res = recover_fut.wait_for(recover_delay);
    if(wait_res == std::future_status::timeout) {
        log->error("Timeout waiting for broker recovery");
        return 1;
    }
    auto recover_res = recover_fut.get();
    if(!recover_res) {
        log->error("Error during broker recovery");
        return 1;
    }

    if(cfg->m_runner_type == cbdc::threepc::runner_type::evm) {
        if(cfg->m_component_id == 0) {
            auto res
                = cbdc::threepc::agent::runner::mint_initial_accounts(log,
                                                                      broker);
            if(!res) {
                log->error("Error minting initial accounts");
                return 1;
            }
        } else {
            log->info("Not seeding, waiting so role 0 can seed");
            static constexpr auto seeding_time = 10;
            std::this_thread::sleep_for(std::chrono::seconds(seeding_time));
        }
    }

    auto server
        = std::unique_ptr<cbdc::threepc::agent::rpc::server_interface>();

    if(cfg->m_runner_type == cbdc::threepc::runner_type::lua) {
        auto rpc_server = std::make_unique<
            cbdc::rpc::async_tcp_server<cbdc::threepc::agent::rpc::request,
                                        cbdc::threepc::agent::rpc::response>>(
            cfg->m_agent_endpoints[cfg->m_component_id]);
        server = std::make_unique<cbdc::threepc::agent::rpc::server>(
            std::move(rpc_server),
            broker,
            log,
            cfg.value());
    } else if(cfg->m_runner_type == cbdc::threepc::runner_type::evm) {
        auto rpc_server = std::make_unique<cbdc::rpc::json_rpc_http_server>(
            cfg->m_agent_endpoints[cfg->m_component_id],
            true);
        server = std::make_unique<cbdc::threepc::agent::rpc::http_server>(
            std::move(rpc_server),
            broker,
            log,
            cfg.value());
    } else {
        log->error("Unknown runner type");
        return 1;
    }

    if(!server->init()) {
        log->error("Error listening on RPC interface");
        return 1;
    }

    static auto running = std::atomic_bool{true};

    std::signal(SIGINT, [](int /* signal */) {
        running = false;
    });

    log->info("Agent running");

    while(running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    log->info("Shutting down...");

    return 0;
}
