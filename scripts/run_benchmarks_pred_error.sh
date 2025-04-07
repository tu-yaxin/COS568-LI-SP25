#! /usr/bin/env bash

echo "Executing benchmark and saving results..."

BENCHMARK=build/benchmark
if [ ! -f $BENCHMARK ]; then
    echo "benchmark binary does not exist"
    exit
fi

function execute_uint64_100M_error() {
    echo "Executing insert+lookup mixed workload with insert-ratio 0.9"
    $BENCHMARK ./data/$1 ./data/$1_ops_2M_0.000000rq_0.500000nl_0.900000i_0m_mix --fence --errors --csv --only $2 # benchmark insert and lookup mix
    echo "Executing insert+lookup mixed workload with insert-ratio 0.1"
    $BENCHMARK ./data/$1 ./data/$1_ops_2M_0.000000rq_0.500000nl_0.100000i_0m_mix --fence --errors --csv --only $2 # benchmark insert and lookup mix
}

mkdir -p ./results
mkdir -p ./results/throughput
mkdir -p ./results/errors

# move the results to the appropriate directory
mv ./results/*.csv ./results/throughput/

for DATA in fb_100M_public_uint64 books_100M_public_uint64 osmc_100M_public_uint64
do
for INDEX in DynamicPGM LIPP # Only DynamicPGM will have non-zero error rate
do
    execute_uint64_100M_error ${DATA} $INDEX
done
done

# move the results to the appropriate directory
mv ./results/*.csv ./results/errors/

echo "===================Benchmarking complete!===================="



for FILE in ./results/errors/*.csv
do
    # Check if file contains 0.000000i to determine workload type
    # For lookup-only workload
    # Remove existing header if present
    if head -n 1 $FILE | grep -q "index_name"; then
        sed -i '1d' $FILE  # Delete the first line
    fi
    # Add the header
    sed -i '1s/^/index_name,build_time_ns,index_size_bytes,avg_op_latency_ns,p50_op_latency_ns,p99_op_latency_ns,p999_op_latency_ns,max_op_latency_ns,std_op_latency_ns,avg_pos_search_overhead,pos_search_latency_ns,avg_pred_error,search_method,value\n/' $FILE

    echo "Header set for $FILE"
done
