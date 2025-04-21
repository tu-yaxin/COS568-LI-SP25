#include "benchmarks/benchmark_hybrid_pgm_lipp.h"

#include "benchmark.h"
#include "benchmarks/common.h"
#include "competitors/hybrid_pgm_lipp.h"

template <int record>
void benchmark_64_hybrid_pgm_lipp(tli::Benchmark<uint64_t>& benchmark, const std::string& filename) {
  if (filename.find("books_100M") != std::string::npos) {
    if (filename.find("0.000000i") != std::string::npos) {
      benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
    } else if (filename.find("mix") == std::string::npos) {
      if (filename.find("0m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,256>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
      } else if (filename.find("1m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,256>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,512>>();
      } else if (filename.find("2m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,256>>();
      }
    } else {
      if (filename.find("0.050000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
      } else if (filename.find("0.500000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
      } else if (filename.find("0.800000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,256>>();
      } else if (filename.find("0.900000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,256>>();
      } else if (filename.find("0.100000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,256>>();
      }
    }
  }
  if (filename.find("fb_100M") != std::string::npos) {
    if (filename.find("0.000000i") != std::string::npos) {
      benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
    } else if (filename.find("mix") == std::string::npos) {
      if (filename.find("0m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,256>>();
      } else if (filename.find("1m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,1024>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,256>>();
      } else if (filename.find("2m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,512>>();
      }
    } else {
      if (filename.find("0.050000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
      } else if (filename.find("0.500000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,512>>();
      } else if (filename.find("0.900000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      } else if (filename.find("0.100000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      }
    }
  }
  if (filename.find("osmc_100M") != std::string::npos) {
    if (filename.find("0.000000i") != std::string::npos) {
      benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
    } else if (filename.find("mix") == std::string::npos) {
      if (filename.find("0m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,1024>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,1024>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,1024>>();
      } else if (filename.find("1m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,256>>();
      } else if (filename.find("2m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,1024>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,1024>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,1024>>();
      }
    } else {
      if (filename.find("0.050000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
      } else if (filename.find("0.500000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      } else if (filename.find("0.800000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,256>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      }else if (filename.find("0.900000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,256>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      } else if (filename.find("0.100000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,256>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,64>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      }
    }
  }
  if (filename.find("wiki_100M") != std::string::npos) {
    if (filename.find("0.000000i") != std::string::npos) {
      benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
      benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
    } else if (filename.find("mix") == std::string::npos) {
      if (filename.find("0m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
      } else if (filename.find("1m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, ExponentialSearch<record>,512>>();
      } else if (filename.find("2m") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,32>>();
      }
    } else {
      if (filename.find("0.050000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,16>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,16>>();
      } else if (filename.find("0.500000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, LinearSearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,32>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
      } else if (filename.find("0.800000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      } else if (filename.find("0.900000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      } else if (filename.find("0.100000i") != std::string::npos) {
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,128>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, InterpolationSearch<record>,512>>();
        benchmark.template Run<HybridPGMLipp<uint64_t, BranchingBinarySearch<record>,128>>();
      }
    }
  }
}

INSTANTIATE_TEMPLATES_MULTITHREAD(benchmark_64_hybrid_pgm_lipp, uint64_t);