#ifndef __OPENCBDC_MML_SERVER_H__
#define __OPENCBDC_MML_SERVER_H__

#include "3pc/mml/interface.hpp"
#include "3pc/mml/message.hpp"

#include "util/rpc/async_server.hpp"
#include "util/rpc/format.hpp"
#include "util/network/connection_manager.hpp"

#include "util/common/variant_overloaded.hpp"

namespace cbdc::threepc::mml::rpc {
    class server {
        public:
            server(interface * impl,
            std::unique_ptr<cbdc::rpc::async_server<request, response>> srv);

            ~server();

            server(const server&) = delete;
            auto operator=(const server&) -> server& = delete;
            server(server&&) = delete;
            auto operator=(const server&&) -> server& = delete;
        
        private:
            using callback_type = std::function<void(std::optional<response>)>;
            using request_type = std::pair<request, callback_type>;

            interface* m_impl;
            std::unique_ptr<cbdc::rpc::async_server<request, response>> m_srv;

            std::vector<std::thread> m_threads;

            auto handle_request() -> bool;
    };
}

#endif