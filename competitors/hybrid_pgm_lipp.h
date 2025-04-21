#ifndef TLI_HYBRID_PGM_LIPP_H
#define TLI_HYBRID_PGM_LIPP_H

#include "base.h"
#include "dynamic_pgm_index.h"
#include "pgm_index.h"
#include "lipp.h"

template <class KeyType, class SearchClass, size_t pgm_error>
class HybridPGMLipp : public Competitor<KeyType, SearchClass> {
 public:
  HybridPGMLipp(const std::vector<int>& params){}
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data, size_t num_threads) {
    std::vector<std::pair<KeyType, uint64_t>> loading_data;
    loading_data.reserve(data.size());
    for (const auto& itm : data) {
      loading_data.emplace_back(itm.key, itm.value);
    }

    uint64_t build_time_pgm =
        util::timing([&] { pgm_ = decltype(pgm_)(); });
    pgm_size = 0;

    uint64_t build_time_lipp =
    util::timing(
        [&] { lipp_.bulk_load(loading_data.data(), loading_data.size()); });

    return build_time_pgm + build_time_lipp;
  }

  size_t EqualityLookup(const KeyType& lookup_key, uint32_t thread_id) const {
    uint64_t value;
    if (!pgm_.find(lookup_key, value)){
        if (!lipp_.find(lookup_key, value)){
            return util::NOT_FOUND;
        }
    }
    return value;
  }

  void Insert(const KeyValue<KeyType>& data, uint32_t thread_id) {
    if (pgm_size_ < 1000000) {//how to set threshold?
        pgm_.insert(data.key, data.value);
        pgm_size_++;
    } else {
        for (auto& entry: pgm_) {//how to enumerate?
            lipp_.Insert(entry);
        }
        pgm_.clear(); //how to clear?
        pgm_size = 0;
    }
  }

  std::string name() const { return "HybridPGMLipp"; }

  std::size_t size() const { return pgm_.size_in_bytes(); }

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
  DynamicPGMIndex<KeyType, uint64_t, SearchClass, PGMIndex<KeyType, SearchClass, pgm_error, 16>> pgm_;
};

#endif  // TLI_DYNAMIC_PGM_H
