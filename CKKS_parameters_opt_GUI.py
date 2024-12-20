import math
import pandas as pd
import sqlite3
import csv
import os
import tkinter as tk
from tkinter import ttk, messagebox

## Based on the user's filtering criteria to output csv file, if the filtering criteria can't be met, it will always
## output the first row in the original databse to the output csv file.

db_file = "big_table_128_precision_final1_f.db"    # SQLite database file, user can change the input file name
table_name = "big_table_128_precision_final1_f"         # SQLite table name, user can change the input table name

output_csv = "filtered_output.csv"  #output CSV file

#function to query the SQLite database
def query_db_with_gui(criteria):
    if not os.path.exists(db_file):
        messagebox.showerror("Error", f"Database file '{db_file}' does not exist.")
        return

    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()

    #build SQL query
    where_clause = " AND ".join(criteria) if criteria else "1"
    query = f"SELECT * FROM {table_name} WHERE {where_clause};"

    cursor.execute(query)
    rows = cursor.fetchall()

    if rows:
        #output to CSV for now
        with open(output_csv, 'w', newline='') as outfile:
            writer = csv.writer(outfile)
            writer.writerow([desc[0] for desc in cursor.description])  # Write header
            writer.writerows(rows)
        messagebox.showinfo(
            "Success",
            f"Filtered rows saved to {output_csv}.\nNumber of rows saved: {len(rows)}"
        )
    else:
        cursor.execute(f"SELECT * FROM {table_name} LIMIT 1;")
        first_row = cursor.fetchall()
        #messagebox.showinfo("No Results", "No rows match the specified criteria.")
        with open(output_csv, 'w', newline='') as outfile:
            writer = csv.writer(outfile)
            writer.writerow([desc[0] for desc in cursor.description])  # Write header
            if first_row:
                writer.writerows(first_row)
                messagebox.showinfo(
                    "No Matches",
                    f"No rows match the specified criteria.\n"
                    f"Saved the first row of the database to {output_csv}."
                )
            else:
                # If the database is empty
                messagebox.showerror(
                    "Error",
                    "The database is empty. No rows to save."
                )
    
    conn.close()

# GUI application
def create_gui():
    def submit():
        criteria = []
        for column, entry in column_entries.items():
            value = entry.get().strip()
            if value:
                criteria.append(f"{column} = '{value}'")
        query_db_with_gui(criteria)

    #create the main window
    root = tk.Tk()
    root.title("OpenFHE Database Query GUI")
    root.geometry("800x600") 

    #connect to the database to fetch column names
    if not os.path.exists(db_file):
        messagebox.showerror("Error", f"Database file '{db_file}' does not exist.")
        return
    
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    cursor.execute(f"PRAGMA table_info({table_name});")
    columns = [col[1] for col in cursor.fetchall()]
    #conn.close()
    
    distinct_values = {}
    for column in columns:
        cursor.execute(f"SELECT DISTINCT {column} FROM {table_name};")
        distinct_values[column] = [""] + sorted(row[0] for row in cursor.fetchall() if row[0] is not None)
    conn.close()
    
    frame = ttk.Frame(root, padding="10")     #create GUI components
    frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
    
    # Configure the root window for resizing
    root.rowconfigure(0, weight=1)
    root.columnconfigure(0, weight=1)
    
    frame.rowconfigure(tuple(range(len(columns) + 1)), weight=1)
    frame.columnconfigure(0, weight=1)  # Column 0 (Labels)
    frame.columnconfigure(1, weight=3)  # Column 1 (Input fields)
    
    ttk.Label(frame, text="Select criteria for each column (or leave blank to skip):").grid(
        row=0, column=0, columnspan=2, sticky=tk.W, pady=5
    )

    column_entries = {}
    for i, column in enumerate(columns, start=1):
        #ttk.Label(frame, text=column, anchor="e").grid(row=i, column=0, sticky=(tk.W, tk.E), padx=10)  # Adjust alignment
        ttk.Label(frame, text=column).grid(row=i, column=0, sticky=(tk.W, tk.E))
        combobox = ttk.Combobox(frame, values=distinct_values[column], state="readonly")
        combobox.grid(row=i, column=1, sticky=(tk.W, tk.E))
        combobox.set("")  # Set default empty value
        column_entries[column] = combobox
        #entry = ttk.Entry(frame)
        #entry.grid(row=i, column=1, sticky=(tk.W, tk.E))
        #column_entries[column] = entry
        
    # Define the 'Clear All' button
    def clear_all():
        """Clear all combobox selections."""
        for combobox in column_entries.values():
            combobox.set("")  # Reset each combobox to its blank option

    submit_button = ttk.Button(frame, text="Search", command=submit)
    #submit_button.grid(row=len(columns) + 1, column=0, columnspan=2, pady=10)
    submit_button.grid(row=len(columns) + 1, column=1, pady=10, sticky=tk.W, padx=(70, 10))

    
    # Add the Clear All button
    clear_button = ttk.Button(frame, text="Clear All", command=clear_all)
    #clear_button.grid(row=len(columns) + 2, column=0, columnspan=2, pady=5)
    clear_button.grid(row=len(columns) + 1, column=0, pady=10, sticky=tk.E, padx=(10, 70))


    for i in range(len(columns) + 2):
        frame.rowconfigure(i, weight=1)
    for j in range(2):
        frame.columnconfigure(j, weight=1)

    root.mainloop()

#run it
create_gui()

