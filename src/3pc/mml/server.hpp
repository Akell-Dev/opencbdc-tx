#ifndef __OPENCBDC_MML_SERVER_H__
#define __OPENCBDC_MML_SERVER_H__

#include "3pc/mml/interface.hpp"
#include "3pc/mml/message.hpp"

#include "util/rpc/async_server.hpp"
#include "util/common/blocking_queue.hpp"
#include "util/common/logging.hpp"
#include "util/common/variant_overloaded.hpp"

#include <thread>

namespace cbdc::threepc::mml::rpc {
    class server {
        public:
            using server_type = std::unique_ptr<cbdc::rpc::async_server<request, response>>;

            server(
                interface* impl,
                server_type srv,
                std::shared_ptr<logging::log> log
            );

            ~server();

            server(const server&) = delete;
            auto operator=(const server&) -> server& = delete;
            server(server&&) = delete;
            auto operator=(const server&&) -> server& = delete;
        
        private:
            using callback_type = std::function<void(std::optional<response>)>;
            using request_type = std::pair<request, callback_type>;

            interface* m_impl;
            server_type m_srv;
            std::shared_ptr<logging::log> m_log;
            blocking_queue<request_type> m_queue;

            std::vector<std::thread> m_threads;

            auto handle_request() -> bool;
    };
}

#endif