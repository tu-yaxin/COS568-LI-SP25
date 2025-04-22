#ifndef TLI_HYBRID_PGM_LIPP_H
#define TLI_HYBRID_PGM_LIPP_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

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
        HybridPGMLipp(const std::vector<int>& params) : pgm(params), lipp(params) {}

        uint64_t Build(const std::vector<KeyValue<KeyType>>& data, size_t num_threads) {
            total_size = data.size();
            uint64_t build_time_lipp = lipp.Build(data, num_threads);

            return build_time_lipp;
        }

    size_t EqualityLookup(const KeyType& lookup_key, uint32_t thread_id) const {
        uint64_t value;
        value = pgm.EqualityLookup(lookup_key, thread_id);
        if (value == util::OVERFLOW){
            value = lipp.EqualityLookup(lookup_key, thread_id);
        } 
        return value;
    }

    uint64_t RangeQuery(const KeyType& lower_key, const KeyType& upper_key, uint32_t thread_id) const {
        uint64_t result = pgm.RangeQuery(lower_key, upper_key, thread_id);
        return result;
      }
    

    // void Insert(const KeyValue<KeyType>& data, uint32_t thread_id) {
    //     if (pgm_size >= 0.05 * total_size) {// should test different thresholds
    //         std::cout << "000000000000000" << std::endl;
    //         std::vector<KeyValue<KeyType>> data;
    //         data = pgm.flush_erase();
    //         for (auto& it: data) {//enumerate pgm
    //             lipp.Insert(KeyValue<KeyType>{it.key, it.value}, 0);
    //         }
    //         std::cout << "111111111111111" << std::endl;
    //         pgm_size = 0;
    //     }
    //     pgm.Insert(data, thread_id);
    //     pgm_size++;
    //     total_size++;
    // }

    void Insert(const KeyValue<KeyType>& data, uint32_t thread_id) {
        pgm.Insert(data, thread_id);
        pgm_size++;
        total_size++;
        if (pgm_size >= 0.05 * total_size) {
          flush();
          std::cout << "Flushing PGM to LIPP, current size: " << pgm_size << std::endl;
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
        return vec;
    }

private:
    size_t pgm_size = 0, total_size;
    void flush() {//backup
        std::vector<KeyValue<KeyType>> data;
        data = pgm.get_data();
        for (const auto& kv : data) {
          lipp.Insert(KeyValue<KeyType>{kv.key, kv.value}, 0);
        }
        pgm_size = 0;
      }
    //DynamicPGMIndex<KeyType, uint64_t, SearchClass, PGMIndex<KeyType, SearchClass, pgm_error, 16>> pgm_;
};

#endif  // TLI_DYNAMIC_PGM_H
