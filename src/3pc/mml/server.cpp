#include "server.hpp"

namespace cbdc::threepc::mml::rpc {
    server::server(
        interface* impl,
        std::unique_ptr<cbdc::rpc::async_server<request, response>> srv
    ) : m_impl(impl),
        m_srv(std::move(srv)) {
    m_srv->register_handler_callback(
        [&]([[maybe_unused]]request req, [[maybe_unused]]callback_type callback) -> bool {
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
        for(auto& t : m_threads) {
            if(t.joinable()) {
                t.join();
            }
        }
    }

    auto server::handle_request() -> bool {
        auto req = request_type();

        auto res = std::visit(
            overloaded {
                [&]([[maybe_unused]]execute_request e_req) -> std::optional<response> {
                    return std::nullopt;
                }
            },
            req.first
        );

        req.second(res);
        return true;
    }

}