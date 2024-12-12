import sqlite3
import os

# filter db to delete the ciphertext levels, only output 

# Input and output database filenames
input_db = "big_table_128_precision_final1.db"  # Replace with the actual input database file
table_name_in = "big_table_128_precision_final1" 
output_db = "big_table_128_precision_final1_f.db"  # Name for the new output database
table_name_out = "big_table_128_precision_final1_f"  # Name for the new output database


def process_database(input_db, output_db):
    # Check if input database exists
    if not os.path.exists(input_db):
        print(f"Input database '{input_db}' does not exist.")
        return

    # Connect to the input database
    conn = sqlite3.connect(input_db)
    cursor = conn.cursor()
    
    # Create the new database connection
    conn_out = sqlite3.connect(output_db)
    cursor_out = conn_out.cursor()
    
    cursor_out.execute(f"ATTACH DATABASE '{input_db}' AS input_db;")
    
    cursor_out.execute(f"DROP TABLE IF EXISTS {table_name_out};")

    #create a new table without ciphertext_level_1 and ciphertext_level_2
    sql_query = f"""
    CREATE TABLE {table_name_out} AS
    SELECT lambda, log2_N, cur_log2_delta, cur_q0, cur_L, log2_precision, CAST(MIN(OpenFHE_precision) AS INTEGER) AS OpenFHE_precision
    FROM input_db.{table_name_in}
    WHERE OpenFHE_precision > 0
    GROUP BY lambda, log2_N, cur_log2_delta, cur_q0, cur_L, log2_precision;
    """

    #print(sql_query)
    cursor_out.execute(sql_query)

    conn_out.commit()

    print(f"Filtered data saved to {output_db}.")

    # Close database connections
    conn.close()
    conn_out.close()
    
        

# Run the process
process_database(input_db, output_db)
