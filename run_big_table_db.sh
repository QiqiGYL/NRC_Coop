#!/bin/bash

# *Don't delete me*

#INPUT_DB_FILE="test_bug.db"  # SQLite database file
#TABLE_NAME="test_bug"  # Input table name

#OPENFHE_EXEC="./bin/benchmark/lib-benchmark-precision_db"  # OpenFHE execution binary

#OUTPUT_DB_FILE="test_bug_output1.db"
#OUTPUT_TABLE="test_bug_output1"  # Output table name


INPUT_DB_FILE="big_table_128.db"  # Input SQLite database file name
TABLE_NAME="big_table_128"  # Input table name

OPENFHE_EXEC="./bin/benchmark/lib-benchmark-precision_db"  # OpenFHE execution binary

OUTPUT_DB_FILE="big_table_128_precision.db"  # Output database file name
OUTPUT_TABLE="big_table_128_precision"  # Output table name

# Ensure the output database is clean
sqlite3 $OUTPUT_DB_FILE <<EOF
DROP TABLE IF EXISTS $OUTPUT_TABLE;
CREATE TABLE $OUTPUT_TABLE (
    lambda INTEGER,
    log2_N INTEGER,
    cur_log2_delta INTEGER,
    cur_q0 INTEGER,
    cur_L INTEGER,
    log2_precision INTEGER,
    ciphertext_level_1 INTEGER,
    ciphertext_level_2 INTEGER,
    OpenFHE_precision INTEGER
);
EOF

# Process each row in the input table
sqlite3 $INPUT_DB_FILE "SELECT lambda, log2_N, cur_log2_delta, cur_q0, cur_L, log2_precision FROM $TABLE_NAME;" | while IFS='|' read -r lambda log2_N cur_log2_delta cur_q0 cur_L log2_precision
do
    echo "Processing row: lambda=$lambda, log2_N=$log2_N, cur_log2_delta=$cur_log2_delta, cur_q0=$cur_q0, cur_L=$cur_L, log2_precision=$log2_precision"

    #execute OpenFHE program and capture the output
    openfhe_output=$($OPENFHE_EXEC $lambda $log2_N $cur_log2_delta $cur_q0 $cur_L $log2_precision 2>&1)

    # Separate valid and error outputs

    #echo "---------start 1-------------"


    valid_output=$(echo "$openfhe_output" | grep -v -e "Error" -e "OpenFHEException")
    error_output=$(echo "$openfhe_output" | grep -e "OpenFHEException" -e "Error")


    #echo "---------start 2-------------"

    if [ -n "$valid_output" ]; then
    #process each line of output from the C++ program
        echo "$valid_output" | while IFS='|' read -r level1 level2 logPrecision; do
        #insert the result of each loop iteration into the output database
            sqlite3 $OUTPUT_DB_FILE <<EOF
                INSERT INTO $OUTPUT_TABLE VALUES (
                $lambda, 
                $log2_N, 
                $cur_log2_delta, 
                $cur_q0, 
                $cur_L, 
                $log2_precision, 
                CAST($level1 AS INTEGER), 
                CAST($level2 AS INTEGER), 
                CAST($logPrecision AS INTEGER)
                );
EOF
    done
    fi

    #echo "---------start 3-------------"
    if [ -n "$error_output" ]; then
        echo -e "$error_output\n"
        continue 
    fi

    # Log success or failure per row in database
    if [ $? -eq 0 ]; then
        echo -e "Row processed successfully.\n"
    else
        echo "Error processing row."
    fi
    
done

echo "Processing complete. Results are stored in $OUTPUT_DB_FILE, table $OUTPUT_TABLE."
