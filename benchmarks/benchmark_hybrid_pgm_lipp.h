#pragma once
#include "benchmark.h"
#include "competitors/dynamic_pgm_index.h"
#include "competitors/lipp.h"

template <typename Searcher>
void benchmark_64_hybrid_pgm_lipp(tli::Benchmark<uint64_t>& benchmark, 
                              bool pareto, const std::vector<int>& params);

template <int record>
void benchmark_64_hybrid_pgm_lipp(tli::Benchmark<uint64_t>& benchmark, const std::string& filename);

