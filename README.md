
# Privacy-Preserving-Library-Project

This project is a privacy-preserving Library that aims to implement and and perform benchmarking of homomorphic methods. It includes secure and optimal FHE parameters computation. 

# Dependancies and Library Installation

### For Python Codes
The following libraries are required for running the Python scripts:
- `sqlite3`: For database operations (typically included with Python by default).
- `tkinter`: For building graphical user interfaces (GUIs).
- `pandas`: For data manipulation and exporting results to CSV.

To install the additional libraries (`tkinter` and `pandas`), use:
```bash
pip install pandas tk
```

### For C++ Codes
The following library is required for running the C++ programs:
- `sqlite3`: For interacting with SQLite databases.

To install the sqlite3 libraries, use:
```bash
sudo apt-get install libsqlite3-dev
```

**Features for TFHE/FHEW Parameters Generation File:**
## Main repository

The main repository contains C++ files for the implementation homomorphic methods

## Installation

Clone the repository: 

```bash
https://git-collab.nrc-cnrc.gc.ca/JeanBelo.Klamti/privacy-preserving-library-project.git
cd privacy-preserving-library-project
```

# File Explanation

- **bounds.csv**: Generated from step 1.

- **big_table.csv**: Generated from step 1.

- **big_table.db**: Convert **big_table.csv** to Sqlite3 database by using Step 3.

- **big_table_128.csv**: Extract `lambda = 128` rows from **big_table.csv**.

- **big_table_128.db**: Convert **big_table_128.csv** to Sqlite3 database by using Step 3.

- **big_table_128_precision_final1.csv**: This CSV file was generated on a remote machine for Step 2. The process was terminated at the parameters: `lambda = 128`, `log2_N = 16`, `delta = 28`, `q0 = 32`, `L = 27`, and `log2_precision = 6`.

- **big_table_128_precision_final1.db**: Convert **big_table_128_precision_final1.csv** to Sqlite3 database by using Step 3.


# Repositories

## Generating Random Ciphertexts in C++

### Files
1. **random_ct.h**: Located in `openfhe-development/src/pke/include/encoding`.
2. **random_ct.cpp**: Located in `openfhe-development/src/pke/lib/encoding`.

### Purpose
These files enable C++ programs to generate random ciphertexts with desired levels by including the `random_ct.h` file. The implementation in `random_ct.cpp` handles the logic for generating the ciphertexts.

### Usage
To use this functionality in your C++ project:
1. Include the `random_ct.h` file in your code:
   ```cpp
   #include "random_ct.h"
   ```

## 1. Generating CKKS Parameter CSV Tables with Python

### Files
1. **CKKS_parameters_utils.py**: Located in `openfhe-development/build`.
2. **CKKS_parameters_table.py**: Located in `openfhe-development/build`.

### Purpose
- **CKKS_parameters_utils.py**: Contains utility functions for performing calculations related to CKKS parameters.
- **CKKS_parameters_table.py**: Imports and uses the functions from `CKKS_parameters_utils.py` to calculate the relationship equations between various CKKS parameters. It then generates two output files: `bounds.csv` and `big_table.csv`.

### Outputs
1. **bounds.csv**: A CSV file with four columns:
   - `lambda`: Security level. Also known as 'SecurityLevel' in OpenFHE.
   - `log2_N`: Logarithm base 2 of the ring dimension. Also known as 'RingDim' in OpenFHE.
   - `max_log2_Q`: Maximum value of log2(Q). Ciphertext Modulus Size `Q=Pq` with `q=q_0*q_1*...*q_L` and `P` is an auxiliary factor. We works with `q_i` suc that `log2(q_i)=log2(delta)` for `i>=1`.
   - `min_log2_delta`: Minimum value of base 2 scaling modulus (`q_i`). Also known as 'ScalingModSize' in OpenFHE.

2. **big_table.csv**: A CSV file with six columns:
   - `lambda`: Security level. Also known as 'SecurityLevel' in OpenFHE.
   - `log2_N`: Logarithm base 2 of the ring dimension. Also known as 'RingDim' in OpenFHE.
   - `log2_delta`: Logarithm base 2 of the scaling modulus (`q_i`). Also known as 'ScalingModSize' in OpenFHE.
   - `q0`: First modulus size. Also known as 'FirstModSize' in OpenFHE.
   - `L`: Multiplicative depth. Also known as 'MultiplicativeDepth' in OpenFHE.
   - `log2_precision`: Logarithm base 2 of the precision bits. 

### Steps to Run
1. Navigate to the `openfhe-development/build` directory.
2. Execute `CKKS_parameters_table.py` using Python:
   ```bash
   python3 CKKS_parameters_table.py
   ```

## 2. Calculate OpenFHE Precision with C++ Code (using CSV)

### Files
1. **lib-benchmark-precision_csv.cpp**: Located at `openfhe-development/benchmark/src`.
2. **run_big_table_csv.sh**: Located at `openfhe-development/build`.

### Prerequisites
Before running the benchmarking program, ensure the following:
- A CSV file named `big_table_128.csv` must exist in the `openfhe-development/build` directory. (The user can manually change the imported CSV file name in `run_big_table_csv.sh`.)

### Purpose
The benchmarking program automates the evaluation of CKKS precision in OpenFHE. The shell script `run_big_table_csv.sh`:
- Executes the `lib-benchmark-precision_csv` binary.
- Reads the input CSV file (`big_table_128.csv`) row by row, passing parameters to OpenFHE.
- Measures CKKS precision for the provided parameters.
- Output the original parameters along with their precision results to the new CSV file (`big_table_128_precision.csv`).

### Outputs
Example **big_table_128_precision.csv** output should have nine columns:
   - `lambda`: Security level. Also known as 'SecurityLevel' in OpenFHE.
   - `log2_N`: Logarithm base 2 of the ring dimension. Also known as 'RingDim' in OpenFHE.
   - `log2_delta`: Logarithm base 2 of the scaling modulus (`q_i`). Also known as 'ScalingModSize' in OpenFHE.
   - `q0`: First modulus size. Also known as 'FirstModSize' in OpenFHE.
   - `L`: Multiplicative depth. Also known as 'MultiplicativeDepth' in OpenFHE.
   - `log2_precision`: Logarithm base 2 of the precision bits.
   - `ciphertext_level_1`: The level of ciphertext 1.
   - `ciphertext_level_2`: The level of ciphertext 2.
   - `OpenFHE_precision`: The precision generated by OpenFHE.

**Warning:** The maximum capacity of a CSV file is approximately 1 million rows. It is unclear what will happen if the program generates more than this limit.

### Steps to Run

#### First-Time Setup
1. Make the shell script executable:
   ```bash
   chmod +x run_big_table_csv.sh
2. Build and run the code:
   ```bash
   cmake ..
   make lib-benchmark-precision_csv
   ./run_big_table_csv.sh

#### After Making Changes to the C++ Code or bash code:
You only need to recompile and rerun:
   ```bash
   make lib-benchmark-precision_csv
   ./run_big_table_csv.sh
   ```

## 3. Convert CSV to SQLite3 Database with Python

### File
- **CKKS_convertDB.py**: Located in `openfhe-development/build`.

### Prerequisites
Before running this script, ensure the following:
- A CSV file must be available as input. The file should be located in the same directory where the script is executed.  (The user can manually change the imported CSV file name in `CKKS_convertDB.py`.)
- The `sqlite3` library must be installed in your Python environment.

### Purpose
The script `CKKS_convertDB.py` simplifies the process of converting a CSV file into a SQLite3 database. It:
1. Takes a CSV file as input.
2. Converts the contents of the CSV into a SQLite3 database file.
3. Saves the output database file with the same name as the input CSV file (but with a `.db` extension).

### Steps to Run
1. Navigate to the `openfhe-development/build` directory.
2. Execute the script using Python:
   ```bash
   python3 CKKS_convertDB.py
   ```

## 4. Calculate OpenFHE Precision with C++ Code (using SQLite3 Database)

### Files
1. **lib-benchmark-precision_db.cpp**: Located at `openfhe-development/benchmark/src`.
2. **run_big_table_db.sh**: Located at `openfhe-development/build`.

### Prerequisites
Before running the benchmarking program, ensure the following:
- A SQLite3 database file named `big_table_128.db` must exist in the `openfhe-development/build` directory. (The user can manually change the imported database file name in `run_big_table_db.sh`.)
- The `sqlite3` library must be installed in your C++ environment.

### Purpose
The benchmarking program automates the evaluation of CKKS precision in OpenFHE, using a SQLite3 database. The shell script `run_big_table_db.sh`:
- Executes the `lib-benchmark-precision_db` binary.
- Reads the input database (`big_table_128.db`) row by row, passing parameters to OpenFHE.
- Measures CKKS precision for the provided parameters.
- Outputs the original parameters along with their precision results to a new SQLite3 database (`big_table_128_precision.db`).

### Outputs
Example **big_table_128_precision.db** output should have nine columns:
   - `lambda`: Security level. Also known as 'SecurityLevel' in OpenFHE.
   - `log2_N`: Logarithm base 2 of the ring dimension. Also known as 'RingDim' in OpenFHE.
   - `log2_delta`: Logarithm base 2 of the scaling modulus (`q_i`). Also known as 'ScalingModSize' in OpenFHE.
   - `q0`: First modulus size. Also known as 'FirstModSize' in OpenFHE.
   - `L`: Multiplicative depth. Also known as 'MultiplicativeDepth' in OpenFHE.
   - `log2_precision`: Logarithm base 2 of the precision bits.
   - `ciphertext_level_1`: The level of ciphertext 1.
   - `ciphertext_level_2`: The level of ciphertext 2.
   - `OpenFHE_precision`: The precision generated by OpenFHE.

### Steps to Run

#### First-Time Setup
1. Make the shell script executable:
   ```bash
   chmod +x run_big_table_db.sh
2. Build and  run the code:
   ```bash
   cmake ..
   make lib-benchmark-precision_db
   ./run_big_table_db.sh

#### After Making Changes to the C++ Code
You only need to recompile and rerun:
   ```bash
   make lib-benchmark-precision_db
   ./run_big_table_db.sh
   ```

## 5. Find The Minimum OpenFHE Precision with Python 

### File
- **filterDB.py**: Located in `openfhe-development/build`.

### Prerequisites
Before running this script, ensure the following:
- A SQLite3 database file named `big_table_128_precision_final1.db` must exist in the `openfhe-development/build` directory. (The user can manually change the imported database file name in `filterDB.py`.)
- The `sqlite3` library must be installed in your Python environment.

### Purpose
The python code `filterDB.py` processes the input database to:
1. Remove the `ciphertext_level_1` and `ciphertext_level_2` columns.
2. Identify the row with the lowest `OpenFHE_precision` for each unique combination of `lambda`, `log2_N`, `delta`, `q0`, `L`, and `log2_precision`.
3. Output the results into a new database file named `big_table_128_precision_final1_f.db`, saved in the same directory.

### Outputs
Example **big_table_128_precision_final1_f.db** output should have seven columns:
   - `lambda`: Security level. Also known as 'SecurityLevel' in OpenFHE.
   - `log2_N`: Logarithm base 2 of the ring dimension. Also known as 'RingDim' in OpenFHE.
   - `log2_delta`: Logarithm base 2 of the scaling modulus (`q_i`). Also known as 'ScalingModSize' in OpenFHE.
   - `q0`: First modulus size. Also known as 'FirstModSize' in OpenFHE.
   - `L`: Multiplicative depth. Also known as 'MultiplicativeDepth' in OpenFHE.
   - `log2_precision`: Logarithm base 2 of the precision bits.
   - `OpenFHE_precision`: The precision generated by OpenFHE.

### Steps to Run
1. Navigate to the `openfhe-development/build` directory.
2. Run the script using Python:
   ```bash
   python3 filterDB.py
   ```

## 6. CKKS Parameters Optimization with Python (Version 1)

### File
- **CKKS_paramters_opt_GUI.py**: Located at `openfhe-development/build`.

### Prerequisites
Before running this script, ensure the following:
- A SQLite database file named `big_table_128_precision_final1_f.db` must exist in the `openfhe-development/build` directory.  (The user can manually change the imported database file name in `CKKS_paramters_opt_GUI.py`.)
- The `sqlite3` and the `tkinter` library must be installed in your Python environment.

### Purpose
The purpose of this script is to enable users to filter data from a database for CKKS parameter optimization. It:
1. Reads the SQLite database (`big_table_128_precision_final1_f.db`)
2. Displays a graphical user interface (GUI) allowing the user to select `lambda`, `log2_N`, `delta`, `q0`, `L`, `log2_precision`, and `OpenFHE_precision` variables they wish to filter.
3. Outputs the filtered results into a CSV file named `filtered_output.csv`, saved in the `openfhe-development/build` directory, based on the user's filtering criteria.

### Outputs
Example **filtered_output.csv** output should have seven columns:
   - `lambda`: Security level. Also known as 'SecurityLevel' in OpenFHE.
   - `log2_N`: Logarithm base 2 of the ring dimension. Also known as 'RingDim' in OpenFHE.
   - `log2_delta`: Logarithm base 2 of the scaling modulus (`q_i`). Also known as 'ScalingModSize' in OpenFHE.
   - `q0`: First modulus size. Also known as 'FirstModSize' in OpenFHE.
   - `L`: Multiplicative depth. Also known as 'MultiplicativeDepth' in OpenFHE.
   - `log2_precision`: Logarithm base 2 of the precision bits.
   - `OpenFHE_precision`: The precision generated by OpenFHE.

> **Note:** This version is for advanced users. Regular users can proceed to Step 7.

### Steps to Run
1. Navigate to the `openfhe-development/build` directory.
2. Ensure the SQLite database file `big_table_128_precision_final1_f.db` is present in the same directory.
3. Execute the script using Python:
   ```bash
   python3 CKKS_paramters_opt_GUI.py
   ```

## 7. CKKS Parameter Optimization with GUI (Version 2)

### File
- **CKKS_paramters_opt_GUI_ver2.py**: Located in `openfhe-development/build`.

### Prerequisites
Before running this script, ensure the following:
- A SQLite database file named `big_table_128_precision_final1_f.db` must exist in the `openfhe-development/build` directory.  (The user can manually change the imported database file name in `CKKS_paramters_opt_GUI.py`.)
- The `sqlite3` and the `tkinter` library must be installed in your Python environment.

### Purpose
The purpose of this script is to enable users to filter data from a database for CKKS parameter optimization. It:
1. Reads the SQLite database file (`big_table_128_precision_final1_f.db`).
2. Displays a graphical user interface (GUI) that allows users to select `lambda`, `L`, and `precision` variables for filtering.
3. Output six CSV files based on the user's filtering criteria for further analysis.

### Outputs
1. **filtered_log2_precision_full.csv**: Contains all rows filtered by `lambda`, `L`, and `log2_precision` variables.
2. **filtered_log2_precision_log2N.csv**: Contains rows from `filtered_log2_precision_full.csv` with distinct `log2_N` values, selecting those with the smallest `log2_delta` and `q0`.
3. **filtered_log2_precision_opt.csv**: Contains one optimal row from `filtered_log2_precision_full.csv` with the smallest `log2_N`, `log2_delta`, and `q0` values.
4. **filtered_openfhe_precision_full.csv**: Contains all rows filtered by `lambda`, `L`, and `openFHE_precision` variables.
5. **filtered_openfhe_precision_log2N.csv**: Contains rows from `filtered_openfhe_precision_full.csv` with distinct `log2_N` values, selecting those with the smallest `log2_delta` and `q0`.
6. **filtered_openfhe_precision_opt.csv**: Contains one optimal row from `filtered_openfhe_precision_full.csv` with the smallest `log2_N`, `log2_delta`, and `q0` values.

Example output CSV file should have seven columns:
   - `lambda`: Security level. Also known as 'SecurityLevel' in OpenFHE.
   - `log2_N`: Logarithm base 2 of the ring dimension. Also known as 'RingDim' in OpenFHE.
   - `log2_delta`: Logarithm base 2 of the scaling modulus (`q_i`). Also known as 'ScalingModSize' in OpenFHE.
   - `q0`: First modulus size. Also known as 'FirstModSize' in OpenFHE.
   - `L`: Multiplicative depth. Also known as 'MultiplicativeDepth' in OpenFHE.
   - `log2_precision`: Logarithm base 2 of the precision bits.
   - `OpenFHE_precision`: The precision generated by OpenFHE.

### Steps to Run
1. Navigate to the `openfhe-development/build` directory.
2. Ensure the SQLite database file `big_table_128_precision_final1_f.db` is present in the same directory.
3. Execute the script using Python:
   ```bash
   python CKKS_paramters_opt_GUI_ver2.py
   ```
   
## 8. Benchmarking with Multi CSV File Outputs （Version 1）

### Files
1. **lib-benchmark-9files_v1.cpp**: Located in `openfhe-development/benchmark/src`.
2. **run_benchmark_9files_v1.sh**: Located in `openfhe-development/build`.
3. **merge1.py**: Located in `openfhe-development/build`.

### Purpose
This program automates the benchmarking process while allowing users to manually change parameters for benchmarking in the `lib-benchmark-8files.cpp` program. It also organizes results into CSV files for analysis. The workflow includes:
1. Running the C++ program (`lib-benchmark-9files_v1.cpp`) via the shell script (`run_benchmark_9files_v1.sh`).
2. Capturing benchmark times and CPU consumption for each function and saving them to a CSV file named `benchmark_time_results.csv`.
3. Generating an organized CSV file with variable columns from the C++ program named `benchmark_notime_results.csv`.
4. Using the Python script (`merge1.py`) to:
   - Merge the two CSV files into one comprehensive file containing variable names, benchmark times, and CPU usage.
   - Separate the time and CPU consumption for each function into smaller, more specific CSV files.

### Steps to Run

#### First-Time Setup
In the `build` directory, execute the following commands:
   ```bash
   chmod +x run_benchmark_8files.sh
   cmake ..
   make lib-benchmark-9files_v1
   ./run_benchmark_9files_v1.sh
   python3 merge1.py
   ```

#### After Making Changes to the C++ Code
You only need to recompile and rerun:
   ```bash
   make lib-benchmark-9files_v1
   ./run_benchmark_9files_v1.sh
   python3 merge1.py
   ```

## 9. Benchmarking with Multi CSV File Outputs (version 2)

### Files
1. **lib-benchmark-9files_v2.cpp**: Located in `openfhe-development/benchmark/src`.
2. **run_benchmark_9files_v2.sh**: Located in `openfhe-development/build`.
3. **merge2.py**: Located in `openfhe-development/build`.

### Prerequisites
Before running this program, ensure the following:
- A CSV file named `benchmark_9files.csv`, or any CSV file generated from step 6, must exist in the `openfhe-development/build` directory, user can manually change the import CSV file name in 'run_benchmark_9files_v2.sh'.

### Purpose
This program automates the benchmarking process with the shell script processes the `benchmark_9files.csv` file row by row, passing the variables to the C++ program (`lib-benchmark-9files_v2.cpp`).  It also organizes results into CSV files for analysis. The workflow includes:
1. Running the C++ program (`lib-benchmark-9files_v2.cpp`) via the shell script (`run_benchmark_9files_v2.sh`).
2. Passing parameters row by row from the input CSV file (`benchmark_9files.csv`) into the C++ program.
3. Generating a CSV file named `benchmark_time_results.csv` that includes benchmark times, CPU consumption, and variable columns from the C++ program.
4. Using the Python script (`merge2.py`) to:
   - Organize the `benchmark_time_results.csv` into a more readable format and output to another CSV file named 'benchmark_full_output.csv'.
   - Separate the time and CPU consumption for each function into smaller, more specific CSV files and store them in a subdirectory named 'split_benchmarks'.

### Steps to Run

#### First-Time Setup
In the `build` directory, execute the following commands:
   ```bash
   chmod +x run_benchmark_9files_v2.sh
   cmake ..
   make lib-benchmark-9files_v2
   ./run_benchmark_9files_v2.sh
   python3 merge2.py
   ```

#### After Making Changes to the C++ Code
You only need to recompile and rerun:
   ```bash
   make lib-benchmark-9files_v2
   ./run_benchmark_9files_v2.sh
   python3 merge2.py
   ```

## Acknowledgements

 - Cheon, J.H., Kim, A., Kim, M., Song, Y.: *Homomorphic encryption for arithmetic of approximate numbers*. In: Advances in Cryptology–ASIACRYPT 2017: 23rd International Conference on the Theory and Applications of Cryptology and Information Security, Hong Kong, China, December 3-7, 2017, Proceedings, Part I 23. pp. 409–437. Springer (2017).  
   [Link to Source](https://doi.org/10.1007/978-3-319-70694-8_15)
- Cheon, J.H., Son, Y., Yhee, D.: *Practical FHE Parameters Against Lattice Attacks*.  
   Cryptology ePrint Archive (2021).  
   [Link to Source](https://eprint.iacr.org/2021/112)
- Gentry, C., Halevi, S., Smart, N.P.: *Homomorphic Evaluation of the AES Circuit*.  
   In: Advances in Cryptology–CRYPTO 2012: 32nd Annual Cryptology Conference, Santa Barbara, CA, USA, August 19-23, 2012. Proceedings. pp. 850–867. Springer (2012).  
   [Link to Source](https://doi.org/10.1007/978-3-642-32009-5_49)
- GitHub, H.: *Security of Approximate-Numbers Homomorphic Encryption* (2013).  
   [Link to Source](https://github.com/microsoft/SEAL)


