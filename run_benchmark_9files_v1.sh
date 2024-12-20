#!/bin/bash


# This script runs the Google Benchmark executable and saves the output to a CSV file.

BENCHMARK_EXECUTABLE="./bin/benchmark/lib-benchmark-9files_v1"  #adjust path if needed
OUTPUT_FILE="benchmark_time_results.csv"

if [ ! -f "$BENCHMARK_EXECUTABLE" ]; then
    echo "Error: Benchmark executable not found after build!"
    exit 1
fi

echo "Running benchmark..."
$BENCHMARK_EXECUTABLE --benchmark_out=$OUTPUT_FILE --benchmark_out_format=csv

echo "Benchmark results saved to $OUTPUT_FILE"
