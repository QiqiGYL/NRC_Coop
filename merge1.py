import pandas as pd

# *Don't delete me*

benchmark_table = pd.read_csv('benchmark_notime_results.csv')
# benchmark_para_results = pd.read_csv('benchmark_para_results.csv', skiprows=9)

with open("benchmark_time_results.csv") as file:
    for i, line in enumerate(file):
        if "name" in line: 
            skip_rows = i
            break

#skip rows until the "name" header row
# benchmark_para_results = pd.read_csv("benchmark_para_results.csv", skiprows=skip_rows)
benchmark_time_results = pd.read_csv("benchmark_time_results.csv", skiprows=skip_rows, keep_default_na=False)

# Clean function names in 'benchmark_para_results.csv' to remove extra info after the function name
benchmark_time_results['name'] = benchmark_time_results['name'].str.split('/').str[0]

merged_df = pd.concat([benchmark_table.reset_index(drop=True), benchmark_time_results[['real_time', 'cpu_time']].reset_index(drop=True)], axis=1)

merged_df = merged_df.loc[:, ~merged_df.columns.str.contains('^Unnamed')]

merged_df.to_csv('benchmark_results_full.csv', index=False)

print("Combined CSV files created successfully!")

# split the combined dataframe
function_names = benchmark_table['Function Name'].unique()

for function_name in function_names:

    function_df = merged_df[merged_df["Function Name"] == function_name]
    
    filename = f"{function_name.replace(' ', '_')}_benchmark_results_full.csv"
    
    function_df.to_csv(filename, index=False)

print("Individual function files created successfully!")
