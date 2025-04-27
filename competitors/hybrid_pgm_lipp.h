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
        HybridPGMLipp(const std::vector<int>& params) : params_(params), pgm(params), lipp(params) {}

        uint64_t Build(const std::vector<KeyValue<KeyType>>& data, size_t num_threads) {
            total_size = data.size();
            buffer.reserve(0.05 * total_size);
            buffer.clear();
            pgm = decltype(pgm)(params_);  //empty pgm
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
    
    void Insert(const KeyValue<KeyType>& data, uint32_t thread_id) {
        pgm.Insert(data, thread_id);
        buffer.push_back(data);
        pgm_size++;
        total_size++;
        if (pgm_size >= 0.05 * total_size) {// should test different thresholds
            //std::cout << "000000000000000" << std::endl;
            //std::vector<KeyValue<KeyType>> data;
            //data = pgm.flush_erase();
            for (auto& it: buffer) {//enumerate same pgm list
                lipp.Insert(it, thread_id);
            }
            //std::cout << "111111111111111" << std::endl;
            buffer.clear();
            pgm = decltype(pgm)(params_); 
            pgm_size = 0;
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

    std::string name() const { return "HybridPGMLipp"; }

    std::size_t size() const { return pgm.size() + lipp.size(); }

private:
    size_t pgm_size = 0, total_size;
    std::vector<int> params_;
    std::vector<KeyValue<KeyType>> buffer;
};

#endif  // TLI_DYNAMIC_PGM_H


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

