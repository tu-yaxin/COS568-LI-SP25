#ifndef TLI_HYBRID_PGM_LIPP_H
#define TLI_HYBRID_PGM_LIPP_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>

#include "../util.h"
#include "base.h"
#include "dynamic_pgm_index.h"
#include "pgm_index.h"
#include "lipp.h"

template <class KeyType, class SearchClass, size_t pgm_error>
class HybridPGMLipp : public Competitor<KeyType, SearchClass> {
    public:
        DynamicPGM<KeyType, SearchClass, pgm_error> pgm;
        Lipp<KeyType> lipp;
        
        // Configurable parameters
        struct Config {
            double buffer_ratio = 0.05;  // Buffer size as ratio of total size
            size_t min_flush_size = 1000;  // Minimum items to trigger flush
            size_t max_flush_size = 10000;  // Maximum items to flush at once
            bool async_flush = true;  // Whether to use async flushing
        } config;

        HybridPGMLipp(const std::vector<int>& params) : params_(params), pgm(params), lipp(params) {
            if (params.size() > 1) config.buffer_ratio = params[1] / 100.0;
            if (params.size() > 2) config.min_flush_size = params[2];
            if (params.size() > 3) config.max_flush_size = params[3];
            if (params.size() > 4) config.async_flush = params[4] != 0;
            
            buffer.reserve(config.min_flush_size);
            if (config.async_flush) {
                flush_thread = std::thread(&HybridPGMLipp::flush_worker, this);
            }
        }

        ~HybridPGMLipp() {
            if (config.async_flush) {
                {
                    std::unique_lock<std::mutex> lock(flush_mutex);
                    should_stop = true;
                }
                flush_cv.notify_one();
                if (flush_thread.joinable()) {
                    flush_thread.join();
                }
            }
        }

        uint64_t Build(const std::vector<KeyValue<KeyType>>& data, size_t num_threads) {
            total_size = data.size();
            buffer.reserve(config.min_flush_size);
            buffer.clear();
            pgm = decltype(pgm)(params_);  //empty pgm
            uint64_t build_time_lipp = lipp.Build(data, num_threads);
            return build_time_lipp;
        }

        size_t EqualityLookup(const KeyType& lookup_key, uint32_t thread_id) const {
            uint64_t value;
            value = lipp.EqualityLookup(lookup_key, thread_id);
            if (value == util::NOT_FOUND) {
                value = pgm.EqualityLookup(lookup_key, thread_id);
            } 
            return value;
        }

        uint64_t RangeQuery(const KeyType& lower_key, const KeyType& upper_key, uint32_t thread_id) const {
            uint64_t result = pgm.RangeQuery(lower_key, upper_key, thread_id);
            return result;
        }
    
        void Insert(const KeyValue<KeyType>& data, uint32_t thread_id) {
            pgm.Insert(data, thread_id);
            
            {
                std::lock_guard<std::mutex> lock(buffer_mutex);
                buffer.push_back(data);
                pgm_size++;
                total_size++;
                
                if (pgm_size >= config.min_flush_size) {
                    if (config.async_flush) {
                        flush_cv.notify_one();
                    } else {
                        flush_buffer();
                    }
                }
            }
        }

        bool applicable(bool unique, bool range_query, bool insert, bool multithread, const std::string& ops_filename) const {
            std::string name = SearchClass::name();
            return name != "LinearAVX" && !multithread;
        }

        std::vector<std::string> variants() const { 
            std::vector<std::string> vec;
            vec.push_back(SearchClass::name());
            vec.push_back(std::to_string(pgm_error));
            vec.push_back(std::to_string(static_cast<int>(config.buffer_ratio * 100)));
            vec.push_back(std::to_string(config.min_flush_size));
            vec.push_back(std::to_string(config.max_flush_size));
            vec.push_back(config.async_flush ? "async" : "sync");
            return vec;
        }

        std::string name() const { return "HybridPGMLipp"; }

        std::size_t size() const { return pgm.size() + lipp.size(); }

    private:
        void flush_buffer() {
            std::vector<KeyValue<KeyType>> items_to_flush;
            {
                std::lock_guard<std::mutex> lock(buffer_mutex);
                if (buffer.empty()) return;
                
                // Take up to max_flush_size items
                size_t flush_size = std::min(buffer.size(), config.max_flush_size);
                items_to_flush.assign(buffer.begin(), buffer.begin() + flush_size);
                buffer.erase(buffer.begin(), buffer.begin() + flush_size);
                pgm_size -= flush_size;
            }

            // Flush items to LIPP
            for (const auto& item : items_to_flush) {
                lipp.Insert(item, 0);  // Using thread_id 0 for background flush
            }
        }

        void flush_worker() {
            while (true) {
                std::unique_lock<std::mutex> lock(flush_mutex);
                flush_cv.wait(lock, [this] { 
                    return should_stop || pgm_size >= config.min_flush_size; 
                });
                
                if (should_stop && buffer.empty()) break;
                
                lock.unlock();
                flush_buffer();
            }
        }

        size_t pgm_size = 0, total_size;
        std::vector<int> params_;
        std::vector<KeyValue<KeyType>> buffer;
        
        // Thread safety
        mutable std::mutex buffer_mutex;
        mutable std::mutex flush_mutex;
        std::condition_variable flush_cv;
        std::thread flush_thread;
        std::atomic<bool> should_stop{false};
};

#endif  // TLI_HYBRID_PGM_LIPP_H


// #pragma once

// #include <algorithm>
// #include <cstdlib>
// #include <iostream>
// #include <vector>

// #include "../util.h"
// #include "dynamic_pgm_index.h"    // ← use the official wrapper
// #include "lipp.h"

// template<class KeyType, class SearchClass, size_t pgm_error>
// class HybridPGMLipp : public Competitor<KeyType, SearchClass> {
// public:
//   HybridPGMLipp(const std::vector<int>& params)
//     : params_(params),
//       pgm_(params),
//       lipp_(params),
//       flush_threshold_( params.size()>1 ? params[1] : 100000 )
//   {
//     buffer.reserve(flush_threshold_);
//   }

//   uint64_t Build(const std::vector<KeyValue<KeyType>>& data, size_t t) {
//     buffer.clear();
//     pgm_ = decltype(pgm_)(params_);         // reset PGM
//     return lipp_.Build(data, t);            // bulk‐load LIPP
//   }

//   void Insert(const KeyValue<KeyType>& kv, uint32_t tid) {
//     pgm_.Insert(kv, tid);                   // keep PGM "warm"
//     buffer.push_back(kv);                  // and record it

//     if (buffer.size() >= flush_threshold_) {
//       // NAIVELY re‐insert everything into LIPP
//       for (auto &b : buffer) {
//         lipp_.Insert(b, tid);
//       }
//       buffer.clear();
//       pgm_ = decltype(pgm_)(params_);       // empty the PGM
//     }
//   }

//   // lookup/range/applicable/name/variants/size stay exactly as before…
// private:
//   std::vector<int> params_;
//   size_t flush_threshold_;
//   std::vector<KeyValue<KeyType>> buffer;
//   DynamicPGM<KeyType, SearchClass, pgm_error> pgm_;  // official wrapper
//   Lipp<KeyType>                      lipp_;
// };

