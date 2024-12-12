
# Privacy-Preserving-Library-Project

This project is a privacy-presrving Library that aims to implement and and perform benchmarking of homomorphic methods. It includes secure and optimal FHE paramters computation. 

# Dependancies and Library Installation

(To do: List all required external library for both Python and C++ Code)

### For Python Codes

### For C++ Codes

# File Explanation

- **big_table_128_precision_final1.csv**: This CSV file is generated from a remote machine. The process was terminated at the parameters: `lambda = 128`, `log2_N = 16`, `delta = 28`, `q0 = 32`, `L = 27`, and `log2_precision = 6`.

- **big_table_128_precision_final1.db**: The SQLite3 database version of the file with the same name.


# Repositories

## 1. Calculate OpenFHE Parameters with Python Code

**List of Required Parameters for FHE (based on OpenFHE):**

- Ring Dimension `N`
- Security level `lambda`
- Precision Bit `p`
- multiplicative Depth `L`
- Scaling Modulus Size `log(delta)`
- First Modulus Size `q_0`
- Ciphertext Modulus Size `Q=Pq` with `q=q_0*q_1*...*q_L` and `P` is an auxiliary factor. We works with `q_i` suc that `log2(q_i)=log2(delta)` for `i>=1`.
- Hamming weight `h`
- Standard Deviation `sigma`

**Description of Parameters compution and Constraints for FHE:**

To generate Parameters for CKKS we need to take into considerations:

- For a given ring dimension `N` and Security Level `lambda`m the upper bound of `log2(Q)` can be computed by the following (**We need to add reference**): 
 
```
N > [(lambda+110)*log2(Q/sigma)]/7.2, i.e.,  log2(Q)< [(7.2 * N) / (lambda + 110)] + log2(sigma)
```
- Given a ciphertext modulus size `log2(Q)` we have the following relation for `log2(q)` and `log2(P)` (**We need to add reference**)

```
log2(q)=4*log2(Q)/5 and log2(P)=log2(Q)/5, i.e., log2(P)=log2(q)/4
```

- Given a ring dimension, a standard deviation `sigma` and a hamming weight `h`, the lower bound of the scaling modulus is given by (**We need to add reference**):
```
log2(delta)>log2(N+2*B_{clean}) with B_{clean}=8 * sigma * N * sqrt(2) + 6 * sigma * sqrt(N) + 16 * sigma * sqrt(h * N)
```
**PS:** There is a relation between the scaling modulus size and the precision bits `p` given by (**We need to add reference**):
```
log2(delta)>=p+log2(B_{clean})
```
- Given a ciphertext modulus size `log2(Q)` and a scaling modulus size `log2(delta)`, the maximum value of the corresponding multiplicative depth is given by (**We need to add reference**):

```
log2(delta)<log2(Q)/(L+1), i.e., max(L)<=log2(Q)/log2(delta)-1
```
- Selected scaling modulus size `log2(delta)` and first modulus size `log2(q_0)` should verify:
```
log2(delta)< log2(q_0)<2*log2(delta)
```

**Features for CKKS Parameters Generation Files:**
Computes:
- **Log2(Q)**: Helps in determining the size of the modulus.
- **log2(delta)**: Related to the decryption error.
- **Precision**: Precision for specific security level and N.
- **L**: Maximum multiplicative depth.
Supports parameter configurations for:
- lambda values = [128, 196]
- log2(N) values = [14, 15]
- Fixed h value of 256
Filters results based on precision levels from [8, 16, 32, 64].

Outputs results to a CSV file.


**Features for BGV Parameters Generation File:**


**Features for TFHE/FHEW Parameters Generation File:**
## Main repository

The main repository contains C++ files for the implementation homomorphic methods

## Installation

Clone the repository: 

```bash
https://git-collab.nrc-cnrc.gc.ca/JeanBelo.Klamti/privacy-preserving-library-project.git
cd privacy-preserving-library-project
```

Install dependencies:
```bash
pip install pandas

```
## Deployment

To deploy this project run

```bash
python CKKS_parameters_draft6.py
```

The results will be saved in ```results.csv``` in the same directory.
## Explanation

---



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

#### After Making Changes to the C++ Code
You only need to recompile and rerun:
   ```bash
   make lib-benchmark-precision_csv
   ./run_big_table_csv.sh
   ```

## 3. Calculate OpenFHE Precision with C++ Code (using SQLite3 Database)

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

## 4. Find The Minimum OpenFHE Precision with Python 

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

### Steps to Run
1. Navigate to the `openfhe-development/build` directory.
2. Run the script using Python:
   ```bash
   python3 filterDB.py
   ```

## 5. CKKS Parameters Optimization with Python

### File
- **CKKS_paramters_opt_GUI.py**: Located at `openfhe-development/build`.

### Prerequisites
Before running this script, ensure the following:
- A SQLite database file named `big_table_128_precision_final1_f.db` must exist in the `openfhe-development/build` directory.  (The user can manually change the imported database file name in `CKKS_paramters_opt_GUI.py`.)
- The `sqlite3` and the `tkinter` library must be installed in your Python environment.

### Purpose
The purpose of this script is to enable users to filter data from a database for CKKS parameter optimization. It:
1. Reads the SQLite database (`big_table_128_precision_final1_f.db`)
2. Displays a graphical user interface (GUI) allowing the user to enter variables they wish to filter.
3. Outputs the filtered results into a CSV file named `filtered_output.csv`, saved in the `openfhe-development/build` directory, based on the user's filtering criteria.

> **Note:** This script is currently a draft. Future updates will include a user-friendly GUI for enhanced interaction.

### Steps to Run
1. Navigate to the `openfhe-development/build` directory.
2. Execute the script using Python:
   ```bash
   python3 CKKS_paramters_opt.py

## 6. Benchmarking with Multi CSV File Outputs

### Files
1. **lib-benchmark-8files.cpp**: Located in `openfhe-development/benchmark/src`.
2. **run_benchmark_8files.sh**: Located in `openfhe-development/build`.
3. **merge.py**: Located in `openfhe-development/build`.

### Purpose
This program automates the benchmarking process and organizes results into CSV files for analysis. The workflow includes:
1. Running the C++ program (`lib-benchmark-8files.cpp`) via the shell script (`run_benchmark_8files.sh`).
2. Capturing benchmark times and CPU consumption for each function and saving them to a CSV file.
3. Generating an organized CSV file with variable columns from the C++ program.
4. Using the Python script (`merge.py`) to:
   - Merge the two CSV files into one comprehensive file containing variable names, benchmark times, and CPU usage.
   - Separate the time and CPU consumption for each function into smaller, more specific CSV files.

### Steps to Run
1. Place the files in their respective directories:
   - `lib-benchmark-8files.cpp` in `benchmark/src`.
   - `run_benchmark_8files.sh` and `merge.py` in `build`.

2. In the `build` directory, execute the following commands:
   ```bash
   chmod +x run_benchmark_8files.sh
   cmake ..
   ./run_benchmark_8files.sh
   python3 merge.py


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


