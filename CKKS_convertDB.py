import pandas as pd
import sqlite3

# This should be your csv file name, change to any name you want to replace
base_name = 'example_3'

csv_file = f'{base_name}.csv'
db_file = f'{base_name}.db'
table_name = base_name

#load the CSV file
df = pd.read_csv(csv_file)

#connect to SQLite database (it will create the file if it doesn't exist)
conn = sqlite3.connect(db_file)
cursor = conn.cursor()

#write DataFrame to SQLite table
df.to_sql(table_name, conn, if_exists='replace', index=False)

print(f"CSV file '{csv_file}' file successfully converted to SQLite database '{db_file}'!")
conn.close()