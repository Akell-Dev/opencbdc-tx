#include <benchmark/benchmark.h>
#include <curl/curl.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <json/json.h>
#include <string>

#define BENCHMARK_UNIT_PRIVATE_DECLARE_F(BaseClass, Method, ThreadCnt)      \
    class BaseClass##_##Method##_Benchmark : public BaseClass {             \
      public:                                                               \
        BaseClass##_##Method##_Benchmark() {                                \
            this->SetName(#BaseClass "/" #Method);                          \
            this->Unit(benchmark::kMillisecond);                            \
            this->ThreadRange(1, std::stoi(#ThreadCnt));                    \
        }                                                                   \
                                                                            \
      protected:                                                            \
        virtual void BenchmarkCase(::benchmark::State&) BENCHMARK_OVERRIDE; \
    };

#define BENCHMARK_F_UNIT(BaseClass, Method, ThreadCnt)             \
    BENCHMARK_UNIT_PRIVATE_DECLARE_F(BaseClass, Method, ThreadCnt) \
    BENCHMARK_REGISTER_F(BaseClass, Method);                       \
    void BENCHMARK_PRIVATE_CONCAT_NAME(BaseClass, Method)::BenchmarkCase

size_t write_data([[maybe_unused]] void* buffer,
                  size_t size,
                  size_t nmemb,
                  [[maybe_unused]] void* userp) {
    return size * nmemb;
}

class test : public benchmark::Fixture {
  protected:
    void SetUp(benchmark::State&) override {}

    void TearDown(benchmark::State&) override {}
};

BENCHMARK_F_UNIT(test, agent1, 64)(benchmark::State& state) {
    auto url_post = "http://localhost:8888";

    for(auto _ : state) {
        for(int i = 0; i < 100; i++) {
            CURL* curl = curl_easy_init();

            if(curl) {
                curl_slist* list = NULL;
                list = curl_slist_append(list,
                                         "Content-Type: application/json");
                curl_easy_setopt(curl, CURLOPT_URL, url_post);
                curl_easy_setopt(curl, CURLOPT_POST, 1L);

                Json::Value payload = Json::Value();
                Json::Value params = Json::Value(Json::arrayValue);
                params.append("0x01A151CC5ED14d110cc0e6b64360913DE9f453F1");
                params.append("pending");
                Json::StreamWriterBuilder m_builder;

                payload["id"] = 1;
                payload["jsonrpc"] = "2.0";
                payload["method"] = "eth_getTransactionCount";
                payload["params"] = params;

                auto _params = Json::writeString(m_builder, payload);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _params.c_str());

                curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
        }
    }
}

auto testGetTransactionCount(benchmark::State& state) {
    auto url_post = "http://localhost:8888";

    for(auto _ : state) {
        for(int i = 0; i < 100; i++) {
            CURL* curl = curl_easy_init();

            if(curl) {
                curl_slist* list = NULL;
                list = curl_slist_append(list,
                                         "Content-Type: application/json");

                curl_easy_setopt(curl, CURLOPT_URL, url_post);
                curl_easy_setopt(curl, CURLOPT_POST, 1L);

                Json::Value payload = Json::Value();
                Json::Value params = Json::Value(Json::arrayValue);
                params.append("0x01A151CC5ED14d110cc0e6b64360913DE9f453F1");
                params.append("pending");
                Json::StreamWriterBuilder m_builder;

                payload["id"] = 1;
                payload["jsonrpc"] = "2.0";
                payload["method"] = "eth_getTransactionCount";
                payload["params"] = params;

                auto _params = Json::writeString(m_builder, payload);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _params.c_str());

                curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
        }
    }
}

BENCHMARK(testGetTransactionCount)->Unit(benchmark::kMillisecond)->ThreadRange(1, 64);

BENCHMARK_MAIN();
