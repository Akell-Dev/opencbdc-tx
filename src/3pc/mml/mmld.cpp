#include "mmld.hpp"

auto main([[maybe_unused]] int argc, [[maybe_unused]]char** argv) -> int {
    auto log = std::make_shared<cbdc::logging::log> (
        cbdc::logging::log_level::trace
    );

    cbdc::network::endpoint_t test = cbdc::network::endpoint_t{"localhost", 9000};
    log->info("MML Resource Monitoring Start...");
    [[maybe_unused]] auto rpc_server = std::make_unique<
        cbdc::rpc::async_tcp_server<cbdc::threepc::mml::rpc::request,
                                    cbdc::threepc::mml::rpc::response>>(
                test
       );

    log->info("Monotoring Shutting down...");
    return 0;
}