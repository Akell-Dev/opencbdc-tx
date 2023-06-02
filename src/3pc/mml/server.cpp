#include "server.hpp"

namespace cbdc::threepc::mml::rpc {
    server::server(
        std::unique_ptr<server_type> srv,
        std::shared_ptr<logging::log> log,
        const cbdc::threepc::mml::options& opts
    ) : m_srv(std::move(srv)),
        m_log(std::move(log)),
        m_opts(opts) {
    // m_srv->register_handler_callback(
    //     [&]([[maybe_unused]]request req, [[maybe_unused]]server_type::callback_type callback) -> bool {
    //         return true;
    //     });
    }

    server::~server() {
        m_log->trace("MML Server shutting down...");
        m_log->trace("Shutdown MML Server");
    }

    auto server::init() -> bool {
        return m_srv->init();
    }
}