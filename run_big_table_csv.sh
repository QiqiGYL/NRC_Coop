#!/bin/bash

# *Don't delete me*

#CSV_FILE="test_bug.csv"  #import csv file
#OPENFHE_EXEC="./bin/benchmark/lib-benchmark-precision_csv"   #execute lib-benchmark-precision_csv
#RESULTS_FILE="test_bug_output1.csv"  #output csv file

CSV_FILE="big_table_128.csv"  #import csv file
OPENFHE_EXEC="./bin/benchmark/lib-benchmark-precision_csv"   #execute lib-benchmark-precision_csv
RESULTS_FILE="big_table_128_precision.csv"  #output csv file

#clear and initialize the results file
echo "lambda,log2_N,cur_log2_delta,cur_q0,cur_L,log2_precision,ciphertext_level_1,ciphertext_level_2,OpenFHE_precision" > "$RESULTS_FILE"

tail -n +2 "$CSV_FILE" | while IFS=',' read -r lambda log2_N cur_log2_delta cur_q0 cur_L log2_precision
do
    echo "Processing row: lambda=$lambda, log2_N=$log2_N, cur_log2_delta=$cur_log2_delta, cur_q0=$cur_q0, cur_L=$cur_L, log2_precision=$log2_precision"

    #start a new OpenFHE process for the current record
    $OPENFHE_EXEC $lambda $log2_N $cur_log2_delta $cur_q0 $cur_L $log2_precision >> "$RESULTS_FILE"

    #log success or failure
    if [ $? -eq 0 ]; then
        echo -e "Row processed successfully.\n"
    else
        echo "Error processing row."
    fi
done