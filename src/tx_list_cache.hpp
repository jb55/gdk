#ifndef GDK_TX_LIST_CACHE_HPP
#define GDK_TX_LIST_CACHE_HPP
#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include <nlohmann/json.hpp>

namespace ga {
namespace sdk {
    class tx_list_cache {
    public:
        using get_txs_fn_t = std::function<std::vector<nlohmann::json>(uint32_t)>;

        std::vector<nlohmann::json> get(uint32_t first, uint32_t count, get_txs_fn_t get_txs);

    private:
        static constexpr uint32_t CACHE_SIZE = 1024;
        uint32_t m_next_uncached_page = 0;
        uint32_t m_first_empty_page = std::numeric_limits<uint32_t>::max();
        std::vector<nlohmann::json> m_tx_cache;
        std::mutex m_mutex;

        bool cache_full();
    };

    class tx_list_caches {
    public:
        void purge_all();
        void purge(uint32_t subaccount);
        std::shared_ptr<tx_list_cache> get(uint32_t subaccount);

    private:
        std::mutex m_mutex;
        std::map<uint32_t, std::shared_ptr<tx_list_cache>> m_caches;
    };

} // namespace sdk
} // namespace ga

#endif
