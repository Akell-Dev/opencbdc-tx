#include "mmld.hpp"

auto main(int argc, char** argv) -> int {
    auto args = cbdc::config::get_args(argc, argv);

    if ( args.size() < 3 ) {
        std::cerr << "Usage: " << args[0] << " <config file> <server mode>"
                << std::endl;
        return 0;
    }

    auto mode = cbdc::threepc::mml::mode::not_supported;
    auto maybe_mode = (std::string) argv[2];
    if ( maybe_mode == "server") {
        mode = cbdc::threepc::mml::mode::server;
        std::cout << "server mode enable" << std::endl;
    } else if ( maybe_mode == "client") {
        mode = cbdc::threepc::mml::mode::client;
        std::cout << "client mode enable" << std::endl;
    } else {
        std::cout << "Active Mode Not Setting, Shut down....";
        return -1;
    }

    auto cfg_or_err = cbdc::threepc::mml::load_options(argv[1]);
    if(std::holds_alternative<std::string>(cfg_or_err)) {
        std::cerr << "Error loading config file: "
                  << std::get<std::string>(cfg_or_err) << std::endl;
        return -1;
    }

    auto opts = std::get<cbdc::threepc::mml::options>(cfg_or_err);

    auto logger = std::make_shared<cbdc::logging::log>(
        opts.m_loglevel
    );

    logger->info("MML Resource Monitoring Start...");
    if(mode == cbdc::threepc::mml::mode::server) {
        logger->trace("SERVER mode");
        cbdc::network::endpoint_t test
            = cbdc::network::endpoint_t{"localhost", 9000};
        [[maybe_unused]] auto rpc_server = std::make_unique<
            cbdc::rpc::async_tcp_server<cbdc::threepc::mml::rpc::request,
                                        cbdc::threepc::mml::rpc::response>>(
            test);
    } else if (mode == cbdc::threepc::mml::mode::client) {
        [[maybe_unused]] cbdc::network::endpoint_t test = cbdc::network::endpoint_t{"localhost", 9001};
        logger->trace("CLIENT mode");
    }

    logger->info("Monotoring Shutting down...");
    return 0;
}