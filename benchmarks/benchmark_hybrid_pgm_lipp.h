#pragma once
#include "benchmark.h"
#include "competitors/dynamic_pgm_index.h"
#include "competitors/lipp.h"

template <typename Searcher>
void benchmark_64_hybrid_pgm_lipp(tli::Benchmark<uint64_t>& benchmark, 
                              bool pareto, const std::vector<int>& params);

template <int record>
void benchmark_64_hybrid_pgm_lipp(tli::Benchmark<uint64_t>& benchmark, const std::string& filename);

// class BenchmarkHybridPGMLipp : public Benchmark {
// public:
//     BenchmarkHybridPGMLipp(size_t lipp_threshold_percent = 5, size_t migration_threshold = 1000000);
//     void Insert(const KeyType& key, const ValueType& value) override;
//     bool Lookup(const KeyType& key) override;
//     void MigrateDataFromDPGMToLIPP();

// private:
//     DynamicPGM dpgm_;
//     LIPPIndex lipp_;
//     size_t lipp_threshold_percent_;
//     size_t migration_threshold_;
//     size_t dpgm_size_;
//     size_t total_inserted_keys_;
// };

// #endif  // BENCHMARK_HYBRID_PGM_LIPP_H_
