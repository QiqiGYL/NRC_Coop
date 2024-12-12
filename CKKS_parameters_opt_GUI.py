import math
import pandas as pd
#from CKKS_Parameters_utils import *
import sqlite3
import csv
import os
import tkinter as tk
from tkinter import ttk, messagebox

db_file = "big_table_128_precision_final1_f.db"    # SQLite database file
table_name = "big_table_128_precision_final1_f"         # SQLite table name
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
        messagebox.showinfo("Success", f"Filtered rows saved to {output_csv}.")
    else:
        messagebox.showinfo("No Results", "No rows match the specified criteria.")
    
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

    #connect to the database to fetch column names
    if not os.path.exists(db_file):
        messagebox.showerror("Error", f"Database file '{db_file}' does not exist.")
        return
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    cursor.execute(f"PRAGMA table_info({table_name});")
    columns = [col[1] for col in cursor.fetchall()]
    conn.close()

    frame = ttk.Frame(root, padding="10")     #create GUI components
    frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))

    ttk.Label(frame, text="Enter criteria for each column (or leave blank to skip):").grid(
        row=0, column=0, columnspan=2, sticky=tk.W, pady=5
    )

    column_entries = {}
    for i, column in enumerate(columns, start=1):
        ttk.Label(frame, text=column).grid(row=i, column=0, sticky=tk.W)
        entry = ttk.Entry(frame, width=30)
        entry.grid(row=i, column=1, sticky=tk.W)
        column_entries[column] = entry

    submit_button = ttk.Button(frame, text="Submit", command=submit)
    submit_button.grid(row=len(columns) + 1, column=0, columnspan=2, pady=10)

    root.mainloop()

#run it
create_gui()

























#function to query SQLite 
'''
def query_db(db_file, table_name="example_3", output_csv="filtered_output.csv"):
    
    if not os.path.exists(db_file):
        print(f"Database file '{db_file}' does not exist.")
        return

    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    
    #get available columns
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
    tables = cursor.fetchall()
    print("Tables in the database:", tables)

    
    cursor.execute(f"PRAGMA table_info({table_name});")
    schema_info = cursor.fetchall()
    #print("Schema Info:", schema_info)
    
    columns = [col[1] for col in schema_info]
    
    #display columns
    print("Available columns in the database:")
    for i, column in enumerate(columns, start=1):
        print(f"{i}. {column}")
    print("\nYou can skip a criterion by pressing Enter.")
    
    #user input filter 
    criteria = []
    for column in columns:
        value = input(f"Enter a value for '{column}' (or press Enter to skip): ").strip()
        if value:
            criteria.append(f"{column} = '{value}'")
    
    #build the SQL query
    if criteria:
        where_clause = " AND ".join(criteria)
        query = f"SELECT * FROM {table_name} WHERE {where_clause};"
    else:
        print("No criteria provided. Exiting.")
        conn.close()
        return
    
    #execute the sqlllllllllllllllll
    cursor.execute(query)
    rows = cursor.fetchall()
    
    if rows:
        print(f"Found {len(rows)} matching rows.")
        
        #output CSV
        with open(output_csv, 'w', newline='') as outfile:
            writer = csv.writer(outfile)
            writer.writerow(columns)  
            writer.writerows(rows)
        
        print(f"Filtered rows saved to {output_csv}.")
    else:
        print("No rows match the specified criteria.")
    
    conn.close()


#execute the function and output csv
query_db(db_file, table_name, output_csv)


#we dont modify this for now
def GetOptParameters(security_level, precision, Circuit_depth):
    """
    This function extract optimal parameters from the CSV file
    Input:
    Output:
    """
    
    return RingDim, SaclingMod, FirstMod, AuxFactor


def ComputeOptParameters(security_level, precision, Circuit_depth):
    """
    This function compute Optimal parameters without using the CSV file
    Input:
    Output:
    """
     
    return 0
def GetMaxDepth():
    """
    Input:
    Output:
    """
    return 0

def GetPrecision():
    """
    This function extract optimal parameters from the CSV file
    Input:
    Output:
    """
    return 0

'''