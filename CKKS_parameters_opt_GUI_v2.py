import pandas as pd
import sqlite3
import csv
import os
import tkinter as tk
from tkinter import ttk, messagebox


db_file = "big_table_128_precision_final1_f.db"    # SQLite database file, user can change
table_name = "big_table_128_precision_final1_f"         # SQLite table name, user can change

output_log2_precision_csv = "filtered_log2_precision_full.csv"
output_openfhe_precision_csv = "filtered_openfhe_precision_full.csv"


## The comment function does similar things, the difference is that the comment function will search through
## the all valid L values starting from the user input L for the given lambda and iterate over precision_value to the minimum.
## That means for example when user input lambda = 128, L = 20, precision = 30, when it couldn't find a match in the db
## it will possibly output math for lambda = 128, L = 15, precision = 30, but we don't want to change the L,
## so this version of the function is in comment.

#    Query the database based on criteria and save the results to a CSV file. If no rows are found,
#    decrement both the L value and precision value iteratively until a match is found or
#    the minimum values for both are reached.

#    Args:
#        criteria (list): A list of SQL conditions for the WHERE clause (excluding L and precision).
#        precision_column (str): The column name for precision filtering.
#        precision_value (int): The initial value of the precision to filter.
#        l_value (int): The initial value of L to filter.
#        output_csv (str): The file name to save the filtered rows.
#        min_precision (int): The minimum precision value to search.
#        min_l (int): The minimum L value to search.

#    Returns:
#        tuple: A tuple containing the number of rows saved, the final L value, and the final precision value used.
"""
def query_db_and_save_to_csv(criteria, precision_column, precision_value, l_value, output_csv, min_precision=1, min_l=1):

    if not os.path.exists(db_file):
        messagebox.showerror("Error", f"Database file '{db_file}' does not exist.")
        return 0, None, None

    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()

    # Build base SQL query
    base_where_clause = " AND ".join(criteria) if criteria else "1"

    current_l = int(l_value)
    rows = []

    # Outer loop: Iterate over L values
    while current_l >= min_l:
        print(f"Trying L = {current_l}...")

        current_precision = int(precision_value)

        # Inner loop: Iterate over precision values
        while current_precision >= min_precision:
            print(f"  Trying {precision_column} = {current_precision}...")

            # Append L and precision conditions
            full_where_clause = f"{base_where_clause} AND L = ? AND {precision_column} = ?"
            query = f"SELECT * FROM {table_name} WHERE {full_where_clause};"

            cursor.execute(query, (current_l, current_precision))
            rows = cursor.fetchall()

            if rows:  # If rows are found, break both loops
                break

            current_precision -= 1  # Decrement precision

        if rows:  # If rows are found, break the outer loop
            break

        current_l -= 1  # Decrement L

    # Save results to CSV if rows are found
    if rows:
        with open(output_csv, 'w', newline='') as outfile:
            writer = csv.writer(outfile)
            writer.writerow([desc[0] for desc in cursor.description])  # Write headers
            writer.writerows(rows)

    conn.close()

    # Return the number of rows saved, the final L, and the precision used
    return len(rows), current_l if rows else None, current_precision if rows else None

"""


def query_db_and_save_to_csv(criteria, precision_column, precision_value, l_value, output_csv):
    """
    Query the database based on criteria, L, and precision, and save the results to a CSV file.

    Args:
        criteria (list): SQL criteria for the query.
        precision_column (str): Column name to query for precision.
        precision_value (int): Starting precision value for the query.
        l_value (int): L value to query for.
        output_csv (str): Path to save the filtered results.

    Returns:
        tuple: (number of rows saved, final L, final precision used)
    """
    if not os.path.exists(db_file):
        messagebox.showerror("Error", f"Database file '{db_file}' does not exist.")
        return 0, None, None

    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()

    # Find valid L range for the given lambda
    l_query = f"SELECT DISTINCT L FROM {table_name} WHERE {' AND '.join(criteria)}"
    cursor.execute(l_query)
    valid_L_values = sorted(row[0] for row in cursor.fetchall())

    if not valid_L_values:
        conn.close()
        return 0, None, None  # No matching L values found

    # Check if the provided L is in range
    if int(l_value) in valid_L_values:
        # If L is valid, iterate over precision only
        for current_precision in range(int(precision_value), 0, -1):
            query = (
                f"SELECT * FROM {table_name} WHERE {' AND '.join(criteria)} AND "
                f"L = ? AND {precision_column} = ?"
            )
            cursor.execute(query, (l_value, current_precision))
            rows = cursor.fetchall()

            if rows:
                # Save the rows to the CSV
                with open(output_csv, 'w', newline='') as outfile:
                    writer = csv.writer(outfile)
                    writer.writerow([desc[0] for desc in cursor.description])  # Write headers
                    writer.writerows(rows)

                conn.close()
                return len(rows), l_value, current_precision


    else:
        # If L is not valid, start from maximum L and iterate
        max_L = max(valid_L_values)
        for current_L in sorted(valid_L_values, reverse=True):  # Iterate from max to min L
            for current_precision in range(int(precision_value), 0, -1):
                query = (
                    f"SELECT * FROM {table_name} WHERE {' AND '.join(criteria)} AND "
                    f"L = ? AND {precision_column} = ?"
                )
                cursor.execute(query, (current_L, current_precision))
                rows = cursor.fetchall()

                if rows:
                    # Save the rows to the CSV
                    with open(output_csv, 'w', newline='') as outfile:
                        writer = csv.writer(outfile)
                        writer.writerow([desc[0] for desc in cursor.description])  # Write headers
                        writer.writerows(rows)

                    conn.close()
                    return len(rows), current_L, current_precision

    # If no match found
    conn.close()
    return 0, None, None


def filter_by_log2N_and_save(input_csv, output_csv):
    """
    Filters rows by distinct log2_N values and selects rows with minimum log2_delta and q0.

    Args:
        input_csv (str): The input CSV file path.
        output_csv (str): The output CSV file path.

    Returns:
        int: The number of rows saved to the output CSV file.
    """
    try:
        # Read the input CSV into a DataFrame
        df = pd.read_csv(input_csv)

        # If the input CSV is empty, return 0 rows
        if df.empty:
            return 0

        # Group by distinct log2_N values and find rows with the minimum log2_delta and q0
        filtered_df = (
            df.loc[df.groupby("log2_N")[["log2_delta", "q0"]].idxmin().values.flatten()]
            .drop_duplicates()  # Ensure distinct rows
        )

        # Save the filtered DataFrame to the output CSV
        filtered_df.to_csv(output_csv, index=False)

        # Return the number of rows saved
        return len(filtered_df)
    except Exception as e:
        print(f"Error processing {input_csv}: {e}")
        return 0
    
def filter_by_opt_and_save(input_csv, output_csv):
    """
    Filters rows by the smallest log2_N value and selects the row with the minimum log2_delta and q0.

    Args:
        input_csv (str): The input CSV file path.
        output_csv (str): The output CSV file path.

    Returns:
        int: The number of rows saved to the output CSV file.
    """
    try:
        # Read the input CSV into a DataFrame
        df = pd.read_csv(input_csv)

        # If the input CSV is empty, return 0 rows
        if df.empty:
            return 0

        # Find the row with the smallest log2_N value
        min_log2_N = df["log2_N"].min()
        filtered_df = df[df["log2_N"] == min_log2_N]

        # Within the rows for the smallest log2_N, find the row with minimum log2_delta and q0
        filtered_row = filtered_df.loc[filtered_df[["log2_delta", "q0"]].idxmin().values[0]]

        # Save the filtered row to the output CSV
        filtered_row.to_frame().T.to_csv(output_csv, index=False)

        # Return the number of rows saved (1 row)
        return 1
    except Exception as e:
        print(f"Error processing {input_csv}: {e}")
        return 0
    

def create_gui():
    """
    Create the GUI for user input with increased vertical spacing and enhanced layout.
    """
    
    def submit():
        # Get user input from the GUI widgets
        lambda_value = lambda_combobox.get()
        l_value = l_combobox.get()
        precision_value = precision_combobox.get()

        # Debug: Print user inputs to confirm what the user selected
        print(f"User inputs: lambda={lambda_value}, L={l_value}, precision={precision_value}")
        
        output_files = [
            "filtered_log2_precision_full.csv",
            "filtered_openfhe_precision_full.csv",
            "filtered_log2_precision_log2N.csv",
            "filtered_openfhe_precision_log2N.csv",
            "filtered_log2_precision_opt.csv",
            "filtered_openfhe_precision_opt.csv",
        ]

        # Remove existing output files
        for file in output_files:
            if os.path.exists(file):
                os.remove(file)
                print(f"Removed existing file: {file}")

        # Validate input
        if not lambda_value or not l_value or not precision_value:
            messagebox.showerror("Error", "All fields must be selected!")
            return

        # Query and save the results for log2_precision
        log2_precision_rows, final_log2_l, final_log2_precision = query_db_and_save_to_csv(
            criteria=[
                f"lambda = {lambda_value}",
            ],
            precision_column="log2_precision",
            precision_value=precision_value,
            l_value=l_value,
            output_csv="filtered_log2_precision_full.csv"
        )

        # Query and save the results for OpenFHE_precision
        openfhe_precision_rows, final_openfhe_l, final_openfhe_precision = query_db_and_save_to_csv(
            criteria=[
                f"lambda = {lambda_value}",
            ],
            precision_column="OpenFHE_precision",
            precision_value=precision_value,
            l_value=l_value,
            output_csv="filtered_openfhe_precision_full.csv"
        )

        log2N_log2_precision_rows = filter_by_log2N_and_save(
            input_csv="filtered_log2_precision_full.csv",
            output_csv="filtered_log2_precision_log2N.csv"
        )

        # Filter openfhe_precision rows by log2_N
        log2N_openfhe_precision_rows = filter_by_log2N_and_save(
            input_csv="filtered_openfhe_precision_full.csv",
            output_csv="filtered_openfhe_precision_log2N.csv"
        )
        
        # Filter log2_precision by smallest log2_N
        opt_log2_precision_row = filter_by_opt_and_save(
            input_csv="filtered_log2_precision_log2N.csv",
            output_csv="filtered_log2_precision_opt.csv"
        )

        # Filter openfhe_precision by smallest log2_N
        opt_openfhe_precision_row = filter_by_opt_and_save(
            input_csv="filtered_openfhe_precision_log2N.csv",
            output_csv="filtered_openfhe_precision_opt.csv"
        )

        # Show success message with the number of rows saved, final L, and final precision used
        log2_message = (
            f"File: filtered_log2_precision_full.csv\n"
            f"Rows: {log2_precision_rows}\n"
            f"Final L: {final_log2_l}\n"
            f"Final precision used: {final_log2_precision}\n\n"
            if log2_precision_rows > 0 else "No matching rows found for log2_precision.\n\n"
        )

        openfhe_message = (
            f"File: filtered_openfhe_precision_full.csv\n"
            f"Rows: {openfhe_precision_rows}\n"
            f"Final L: {final_openfhe_l}\n"
            f"Final precision used: {final_openfhe_precision}\n\n"
            if openfhe_precision_rows > 0 else "No matching rows found for OpenFHE_precision.\n\n"
        )
            
        log2N_log2_message = (
            f"File: filtered_log2_precision_log2N.csv\n"
            f"Rows: {log2N_log2_precision_rows}\n\n"
            if log2N_log2_precision_rows > 0 else "No matching rows found for log2_precision filtered by log2_N.\n\n"
        )

        log2N_openfhe_message = (
            f"File: filtered_openfhe_precision_log2N.csv\n"
            f"Rows: {log2N_openfhe_precision_rows}\n\n"
            if log2N_openfhe_precision_rows > 0 else "No matching rows found for OpenFHE_precision filtered by log2_N.\n\n"
        )
        
        # Update messagebox to include information about the new files
        opt_log2_precision_message = (
            f"File: filtered_log2_precision_opt.csv\n"
            f"Rows: {opt_log2_precision_row}\n\n"
            if opt_log2_precision_row > 0 else "No matching rows found for log2_precision filtered by minimum log2_N.\n\n"
        )

        opt_openfhe_message = (
            f"File: filtered_openfhe_precision_opt.csv\n"
            f"Rows: {opt_openfhe_precision_row}\n"
            if opt_openfhe_precision_row > 0 else "No matching rows found for OpenFHE_precision filtered by minimum log2_N.\n"
        )

        messagebox.showinfo(
        "Success",
        f"Filtered rows saved:\n\n{log2_message}{openfhe_message}{log2N_log2_message}{log2N_openfhe_message}"
        f"{opt_log2_precision_message}{opt_openfhe_message}"
    )

    
    # Fetch distinct values from the database
    if not os.path.exists(db_file):
        messagebox.showerror("Error", f"Database file '{db_file}' does not exist.")
        return

    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()

    distinct_values = {}
    for column in ["lambda", "L", "log2_precision", "OpenFHE_precision"]:
        cursor.execute(f"SELECT DISTINCT {column} FROM {table_name};")
        distinct_values[column] = sorted(row[0] for row in cursor.fetchall() if row[0] is not None)
    conn.close()

    # Create the GUI window
    root = tk.Tk()
    root.title("OpenFHE Database Query GUI")
    root.geometry("600x500")  # Increased size for better visibility

    ttk.Label(root, text="Select Lambda:", font=("Arial", 12)).pack(pady=(20, 5))  # More padding above and below
    lambda_combobox = ttk.Combobox(root, values=distinct_values["lambda"], state="readonly", font=("Arial", 10))
    lambda_combobox.pack(pady=(10, 20))  # Increased vertical spacing

    ttk.Label(root, text="Select L:", font=("Arial", 12)).pack(pady=(20, 5))
    l_combobox = ttk.Combobox(root, values=distinct_values["L"], state="readonly", font=("Arial", 10))
    l_combobox.pack(pady=(10, 20))

    ttk.Label(root, text="Select Precision:", font=("Arial", 12)).pack(pady=(20, 5))
    precision_combobox = ttk.Combobox(root, values=distinct_values["log2_precision"], state="readonly", font=("Arial", 10))
    precision_combobox.pack(pady=(10, 20))

    ttk.Button(root, text="Submit", command=submit, width=20).pack(pady=(60, 20))  # More spacing around the button

    root.mainloop()

# Run the GUI
create_gui()
