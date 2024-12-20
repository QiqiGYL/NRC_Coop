#!/bin/bash

# *Don't delete me*

# This script runs the Google Benchmark executable and saves the output to a CSV file.

INPUT_CSV="benchmark_9files.csv" #user can change
BENCHMARK_EXECUTABLE="./bin/benchmark/lib-benchmark-9files_v2"  #adjust path if needed

TERMINAL_OUTPUT_FILE="benchmark_terminal_results.csv"
ACTUAL_RESULTS_FILE="benchmark_time_results.csv"

# Check if the input CSV file exists
if [ ! -f "$INPUT_CSV" ]; then
    echo "Error: Input CSV file '$INPUT_CSV' not found!"
    exit 1
fi

# Check if the executable file exists
if [ ! -f "$BENCHMARK_EXECUTABLE" ]; then
    echo "Error: Benchmark executable not found after build!"
    exit 1
fi

echo "Running benchmark..."

rm -f "$TERMINAL_OUTPUT_FILE" "$ACTUAL_RESULTS_FILE" 

#clear and initialize the results file
echo "Function_name, lambda,log2_N,log2_delta,q0,L,log2_precision,ciphertext_level_1,ciphertext_level_2,OpenFHE_precision" > "$ACTUAL_RESULTS_FILE"

tail -n +2 "$INPUT_CSV" | while IFS=',' read -r lambda log2_N log2_delta q0 L log2_precision OpenFHE_precision
do
    echo "Processing row: lambda=$lambda, log2_N=$log2_N, log2_delta=$log2_delta, q0=$q0, L=$L, log2_precision=$log2_precision, OpenFHE_precision=$OpenFHE_precision"

    #start a new OpenFHE process for the current record
    #$BENCHMARK_EXECUTABLE $lambda $log2_N $log2_delta $q0 $L $log2_precision $OpenFHE_precision>> "$ACTUAL_RESULTS_FILE"
    #$BENCHMARK_EXECUTABLE $lambda $log2_N $log2_delta $q0 $L $log2_precision $OpenFHE_precision --benchmark_out_format=csv >> "$TERMINAL_OUTPUT_FILE"
    
    # Run the benchmark executable for time results
    $BENCHMARK_EXECUTABLE $lambda $log2_N $log2_delta $q0 $L $log2_precision $OpenFHE_precision --benchmark_out="$TERMINAL_OUTPUT_FILE" --benchmark_out_format=csv

    $BENCHMARK_EXECUTABLE $lambda $log2_N $log2_delta $q0 $L $log2_precision $OpenFHE_precision >> "$ACTUAL_RESULTS_FILE"

    #log success or failure
    if [ $? -eq 0 ]; then
        echo -e "Row processed successfully.\n"
    else
        echo -e "Error processing row.\n"
    fi
done


echo "Benchmark results saved to $ACTUAL_RESULTS_FILE"
