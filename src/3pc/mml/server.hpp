#ifndef __OPENCBDC_MML_SERVER_H__
#define __OPENCBDC_MML_SERVER_H__

#include "3pc/mml/interface.hpp"
#include "3pc/mml/message.hpp"
#include "3pc/mml/options.hpp"

#include "util/rpc/tcp_server.hpp"
#include "util/common/thread_pool.hpp"
#include "util/common/blocking_queue.hpp"

#include <atomic>
#include <memory>
#include <secp256k1.h>
#include <thread>

namespace cbdc::threepc::mml::rpc {
    class server {
        public:
            using server_type = cbdc::rpc::async_tcp_server<request, response>;

            server(
                std::unique_ptr<server_type> srv,
                std::shared_ptr<logging::log> log,
                const cbdc::threepc::mml::options& opt
            );

            ~server();

            server(const server&) = delete;
            auto operator=(const server&) -> server& = delete;
            server(server&&) = delete;
            auto operator=(const server&&) -> server& = delete;

            auto init() -> bool;
        
        private:
            std::unique_ptr<server_type> m_srv;
            std::shared_ptr<logging::log> m_log;
            const cbdc::threepc::mml::options& m_opts;

            mutable std::mutex m_mml_mut;
            std::atomic<size_t> m_next_id;
            // std::unordered_map<size_t, std::shared_ptr<mml::impl>> m_mmls;

            blocking_queue<size_t> m_clean_queue;
            std::thread m_clean_thread;
            
            blocking_priority_queue<size_t, std::greater<>> m_retry_queue;
            std::thread m_retry_thread;

            std::shared_ptr<thread_pool> m_threads{
                std::make_shared<thread_pool>()
            };

            std::shared_ptr<secp256k1_context> m_secp {
                secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY ),
                &secp256k1_context_destroy
            };
    };
}

#endif