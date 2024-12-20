import csv
import re
import os

# Input and output file paths
input_file = "benchmark_time_results.csv"
output_file = "benchmark_full_output.csv"
split_output_dir = "split_benchmarks" 

# Ensure the directory for split files exists
os.makedirs(split_output_dir, exist_ok=True)

# Regex to extract time (e.g., 5511 us)
time_pattern = r"(\d+)\s+us"

# Function to check if a row is relevant (non-separator, non-header)
def is_valid_data_row(row):
    '''
    return (
        len(row) > 0 and
        row[0] and
        not "/" in row[0] and
        not row[0].startswith("-") and
        not row[0].startswith("Benchmark")
    )
    '''
    if not row or len(row) < 1:
        return False
    first_cell = row[0]
    #print(f"Now Processing function name: {first_cell}")
    return not (
        first_cell.startswith("-") or  # Separator rows
        first_cell.startswith("Benchmark")  # Header-like rows
    )

    
    
# Open the input CSV file
with open(input_file, mode="r") as infile:
    reader = list(csv.reader(infile))
    header = reader[0]
    data = reader[1:]  
    
# Extend the header with new columns
header.extend(["real_time", "cpu_time"])

# Prepare the final output rows
output_data = [header]  

# Prepare a dictionary to hold split data
split_data = {}

# Iterate through rows and process only valid data rows
for i, row in enumerate(data):
    if is_valid_data_row(row):
        # Check if the row is a function row (no `/` in the name)
        #if not any("/" in cell for cell in row):
        if "/" not in row[0]: 
            # Extract relevant function details
            function_name = row[0]
            print(f"Processing function name: {function_name}")
            lambda_val = row[1]
            log2_N = row[2]
            log2_delta = row[3]
            q0 = row[4]
            L = row[5]
            log2_precision = row[6]
            ciphertext_level_1 = row[7]
            ciphertext_level_2 = row[8]
            OpenFHE_precision = row[9]

            # Initialize real_time and cpu_time
            real_time, cpu_time = "", ""

            # Find the next valid row with `/` and extract times
            for j in range(i + 1, len(data)):
                next_row = data[j]
                if is_valid_data_row(next_row) and "/" in next_row[0]:
                    match = re.findall(time_pattern, next_row[0])
                    if match and len(match) >= 2:
                        real_time = f"{match[0]} us"
                        cpu_time = f"{match[1]} us"
                    break

            output_data.append(
                [
                    function_name,
                    lambda_val,
                    log2_N,
                    log2_delta,
                    q0,
                    L,
                    log2_precision,
                    ciphertext_level_1,
                    ciphertext_level_2,
                    OpenFHE_precision,
                    real_time,
                    cpu_time,
                ]
            )
            #print(f"Processing function name: {function_name}")
            
            # Add to the split data for the corresponding function name
            if function_name not in split_data:
                print(f"New function name encountered: {function_name}")
                split_data[function_name] = [header]
            split_data[function_name].append(
                [
                    function_name,
                    lambda_val,
                    log2_N,
                    log2_delta,
                    q0,
                    L,
                    log2_precision,
                    ciphertext_level_1,
                    ciphertext_level_2,
                    OpenFHE_precision,
                    real_time,
                    cpu_time,
                ]
            )


# Write the final output CSV
with open(output_file, mode="w", newline="") as outfile:
    writer = csv.writer(outfile)
    writer.writerows(output_data)

print(f"Processed data has been written to {output_file}")

# Write split benchmark CSV files
for function_name, rows in split_data.items():
    sanitized_name = function_name.replace(" ", "_")
    split_file_path = os.path.join(split_output_dir, f"{sanitized_name}_benchmark_results.csv")
    with open(split_file_path, mode="w", newline="") as split_file:
        writer = csv.writer(split_file)
        writer.writerows(rows)
        
print(f"Split benchmark files have been written to the '{split_output_dir}' directory.")
