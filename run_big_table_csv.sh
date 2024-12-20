#!/bin/bash

CSV_FILE="big_table_128.csv"  #import csv file, user can change the import address

OPENFHE_EXEC="./bin/benchmark/lib-benchmark-precision_csv"   #execute lib-benchmark-precision_csv
RESULTS_FILE="big_table_128_precision.csv"  #output csv file

#clear and initialize the results file
echo "lambda,log2_N,log2_delta,q0,L,log2_precision,ciphertext_level_1,ciphertext_level_2,OpenFHE_precision" > "$RESULTS_FILE"

tail -n +2 "$CSV_FILE" | while IFS=',' read -r lambda log2_N log2_delta q0 L log2_precision
do
    echo "Processing row: lambda=$lambda, log2_N=$log2_N, log2_delta=$log2_delta, q0=$q0, L=$L, log2_precision=$log2_precision"

    #start a new OpenFHE process for the current record
    $OPENFHE_EXEC $lambda $log2_N $log2_delta $q0 $L $log2_precision >> "$RESULTS_FILE"

    #log success or failure
    if [ $? -eq 0 ]; then
        echo -e "Row processed successfully.\n"
    else
        echo "Error processing row."
    fi
done