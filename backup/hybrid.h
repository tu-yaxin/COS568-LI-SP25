// #ifndef TLI_DYNAMIC_PGM_H
#define TLI_DYNAMIC_PGM_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "../util.h"
#include "dynamic_pgm_index.h"
#include "lipp.h"

template <class KeyType, class SearchClass, size_t pgm_error>
class HybridPGMLipp : public Competitor<KeyType, SearchClass> {
  public:
    DynamicPGM<KeyType, SearchClass, pgm_error> pgm;
    Lipp<KeyType> lipp;
    HybridPGMLipp(const std::vector<int>& params)
        : pgm(params), lipp(params) {}

  uint64_t Build(const std::vector<KeyValue<KeyType>>& data, size_t num_threads) {
    uint64_t build_time = lipp.Build(data, num_threads);
    total_size = data.size();

    return build_time;
  }

  size_t EqualityLookup(const KeyType& lookup_key, uint32_t thread_id) const {
    uint64_t value;
    value = lipp.EqualityLookup(lookup_key, thread_id);
    if (value == util::OVERFLOW) {
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
    pgm_size++;
    total_size++;
    if (pgm_size >= 0.05 * total_size) {
      flush();
      std::cout << "Flushing PGM to LIPP, current size: " << pgm_size << std::endl;
    }
  }

  std::string name() const { return "HybridPGMLIPP"; }

  std::size_t size() const { return pgm.size() + lipp.size(); }

  bool applicable(bool unique, bool range_query, bool insert, bool multithread, const std::string& ops_filename) const {
    std::string name = SearchClass::name();
    return name != "LinearAVX" && unique && !multithread;
  }

  std::vector<std::string> variants() const {
    return std::vector<std::string>();
  }

  private:
   size_t total_size;
   size_t pgm_size = 0;
   void flush() {
      std::vector<KeyValue<KeyType>> data;
      data = pgm.get_data();
      for (const auto& kv : data) {
        lipp.Insert(KeyValue<KeyType>{kv.key, kv.value}, 0);
      }
      pgm_size = 0;
    }
};

// #endif