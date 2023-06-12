#include "server.hpp"

namespace cbdc::threepc::mml::rpc {
    server::server(interface* impl,
                   server_type srv,
                   std::shared_ptr<logging::log> log)
        : m_impl(impl),
          m_srv(std::move(srv)),
          m_log(std::move(log)) {
        m_srv->register_handler_callback(
            [&]([[maybe_unused]] request req,
                [[maybe_unused]] callback_type callback) -> bool {
                return true;
            });

        auto n_threads = std::thread::hardware_concurrency();
        for(size_t i{0}; i < n_threads; i++) {
            m_threads.emplace_back([&]() {
                while(handle_request()) {}
            });
        }
    }

    server::~server() {
        m_log->trace("MML Server shutting down...");
        m_queue.clear();
        for(auto& t : m_threads) {
            if(t.joinable()) {
                t.join();
            }
        }
        m_log->trace("Shutdown MML Server");
    }

    auto server::handle_request() -> bool {
        auto req = request_type();
        auto popped = m_queue.pop(req);

        if(!popped) {
            return false;
        }

        [[maybe_unused]]auto res = std::visit(
            overloaded{
                [&]([[maybe_unused]]execute_request e_req) -> std::optional<response> {
                    [[nodiscard]]std::move(e_req);
                    return std::nullopt;
                }
            },
            req.first);

        // req.second(res);

        return true;
    }
}