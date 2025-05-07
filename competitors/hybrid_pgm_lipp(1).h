// #ifndef TLI_DYNAMIC_PGM_H
#define TLI_DYNAMIC_PGM_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "../util.h"
#include "dynamic_pgm_index.h"
#include "lipp.h"

template <class KeyType, class SearchClass, size_t pgm_error>
class HybridPGMLipp : public Competitor<KeyType, SearchClass> {
  public:
    DynamicPGM<KeyType, SearchClass, pgm_error> pgm1;
    DynamicPGM<KeyType, SearchClass, pgm_error> pgm2;
    Lipp<KeyType> lipp1;
    Lipp<KeyType> lipp2;
    HybridPGMLipp(const std::vector<int>& params)
        : pgm1(params), pgm2(params), lipp1(params), lipp2(params) {}

    ~HybridPGMLipp() {
      stop_flush_thread();
    }

  uint64_t Build(const std::vector<KeyValue<KeyType>>& data, size_t num_threads) {
    total_size = data.size();
    uint64_t build_time1 = lipp1.Build(data, num_threads);
    uint64_t build_time2 = lipp2.Build(data, num_threads);

    return build_time1 + build_time2;
  }

  size_t EqualityLookup(const KeyType& lookup_key, uint32_t thread_id) const {
    uint64_t value;
    while (!lookup_insert_ok.load()) {
      std::this_thread::yield();
    }
    value = rw_pgm_ptr->EqualityLookup(lookup_key, thread_id);
    if (value == util::OVERFLOW) {
      value = r_pgm_ptr->EqualityLookup(lookup_key, thread_id);
      if (value == util::OVERFLOW) {
        value = r_lipp_ptr->EqualityLookup(lookup_key, thread_id);
      }
    }

    return value;
  }

  uint64_t RangeQuery(const KeyType& lower_key, const KeyType& upper_key, uint32_t thread_id) const {
    uint64_t result = rw_pgm_ptr->RangeQuery(lower_key, upper_key, thread_id);
    return result;
  }

  void Insert(const KeyValue<KeyType>& data, uint32_t thread_id) {
    while (!lookup_insert_ok.load()) {
      std::this_thread::yield();
    }
    rw_pgm_ptr->Insert(data, thread_id);
    // pgm1_size++;
    // total_size++;
    // if (pgm1_size >= 0.001 * total_size) {
    if (flush_complete.load()) {
      flush_mutex.lock();
      flush_complete.store(false);
      lookup_insert_mutex.lock();
      lookup_insert_ok.store(false);
      lookup_insert_mutex.unlock();

      uint64_t tmp = pgm1_size;
      pgm1_size = pgm2_size;
      pgm2_size = 0;

      DynamicPGM<KeyType, SearchClass, pgm_error>* tmp_ptr = &(*rw_pgm_ptr);
      rw_pgm_ptr = &(*r_pgm_ptr);
      r_pgm_ptr = &(*tmp_ptr);

      start_flush_thread();
    }
  }

  std::string name() const { return "HybridPGMLIPP"; }

  std::size_t size() const { return pgm1.size() + pgm2.size() + lipp1.size() + lipp2.size(); }

  bool applicable(bool unique, bool range_query, bool insert, bool multithread, const std::string& ops_filename) const {
    std::string name = SearchClass::name();
    return name != "LinearAVX" && unique;
  }

  std::vector<std::string> variants() const {
    return std::vector<std::string>();
  }

  private:
    size_t total_size;
    size_t pgm1_size = 0;
    size_t pgm2_size = 0;
    DynamicPGM<KeyType, SearchClass, pgm_error>* rw_pgm_ptr = &pgm1;
    DynamicPGM<KeyType, SearchClass, pgm_error>* r_pgm_ptr = &pgm2;
    Lipp<KeyType>* w_lipp_ptr = &lipp1;
    Lipp<KeyType>* r_lipp_ptr = &lipp2;
    std::mutex lookup_insert_mutex;
    std::mutex flush_mutex;
    std::atomic<bool> lookup_insert_ok{true};
    std::atomic<bool> flush_complete{true};
    std::atomic<bool> stop_flush{false};
    std::thread flush_thread;

    void start_flush_thread() {
      stop_flush_thread();

      flush_thread = std::thread(&HybridPGMLipp::flush, this);
      flush_thread.detach();
    }

    void stop_flush_thread() {
      stop_flush.store(true);
      if (flush_thread.joinable()) {
        flush_thread.join();
      }
      stop_flush.store(false);
    }

    void flush() {
      // std::cout << std::this_thread::get_id() << "Flushing start." << std::endl;
      lookup_insert_mutex.lock();
      lookup_insert_ok.store(true);
      lookup_insert_mutex.unlock();

      std::vector<KeyValue<KeyType>> data;
      data = r_pgm_ptr->get_data();
      for (const auto& kv : data) {
        if (stop_flush.load()) {
          return;
        }
        w_lipp_ptr->Insert(KeyValue<KeyType>{kv.key, kv.value}, 0);
      }

      lookup_insert_mutex.lock();
      lookup_insert_ok.store(false);
      lookup_insert_mutex.unlock();

      Lipp<KeyType>* tmp_ptr = &(*w_lipp_ptr);
      w_lipp_ptr = &(*r_lipp_ptr);
      r_lipp_ptr = &(*tmp_ptr);

      lookup_insert_mutex.lock();
      lookup_insert_ok.store(true);
      lookup_insert_mutex.unlock();
      // std::cout << std::this_thread::get_id() << "Flushing complete." << std::endl;
      // std::cout << std::this_thread::get_id() << "Post flushing starts" << std::endl;

      for (const auto& kv : data) {
        w_lipp_ptr->Insert(KeyValue<KeyType>{kv.key, kv.value}, 0);
      }

      r_pgm_ptr->clear();
      flush_complete.store(true);
      flush_mutex.unlock();
      // std::cout << std::this_thread::get_id() << "Post flushing complete." << std::endl;
    }
};

// #endif