import pandas as pd
from CKKS_parameters_utils import *

sigma = 3.19       # Standard deviation σ
max_L = 32          # Maximum multiplication depth
log2_qi_upper = 58  
log2_q0_upper = 60

parameters = {
    #'lambda': [128],
    'lambda': [128, 196, 256],
    'log2_N_values': [14, 15, 16, 17],
    'h_values': [256]
    }

def main():
    # Dictionary to collect results
    bounds = {'lambda': [], 'log2_N': [], 'max_log2_Q': [],
                    'min_log2_delta': []}
    
    big_table = {'lambda': [], 'log2_N': [], 'log2_delta': [], 'q0': [],
                    'L': [], 'log2_precision': []}

    # Output the bounds value
    for lambda_sec in parameters['lambda']:
        for log2_N in parameters['log2_N_values']:
            N = 2 ** log2_N
            for h in parameters['h_values']:
                try:
                    # Task 1: Compute upper bound for Q
                    log_Q = compute_upper_bound_Q(lambda_sec, N)
                    
                    # Task 4: Compute lower bound of log2(delta)
                    log2_delta = log2_delta_bound_1(N, h)
                    
                    if log2_delta > 58:
                        raise ValueError("Lower bound of log2(delta) exceeds 58; no valid solution exists.")
                    
                    bounds['lambda'].append(lambda_sec)
                    bounds['log2_N'].append(log2_N)
                    bounds['max_log2_Q'].append(log_Q)
                    bounds['min_log2_delta'].append(log2_delta)
                            
                except ValueError as e:
                    print(f"Error for lambda={lambda_sec}, N={N}, h={h}: {e}")
                 
    #Output the big table           
    for lambda_sec in parameters['lambda']:
        for log2_N in parameters['log2_N_values']:
            N = 2 ** log2_N
            for h in parameters['h_values']:
                try:
                    # Task 1: Compute upper bound for Q
                    max_log_Q = compute_upper_bound_Q(lambda_sec, N)
                    
                    # Task 3: Compute B_clean value
                    B_clean = compute_B_clean(N, h)

                    # Task 4: Compute lower bound of log2(delta)
                    log2_delta = log2_delta_bound_1(N, h)
                    
                    if log2_delta > log2_qi_upper:
                        raise ValueError("Lower bound of log2(delta) exceeds 58; no valid solution exists.")
                    
                    lowest_log2_qi = min(log2_delta, log2_qi_upper)
                    
                    #for each qi value in [lowest qi, 58]
                    for delta in range(lowest_log2_qi, log2_qi_upper + 1):
                        #for each q0 value such that delta < q0 < delta^2
                        for q0 in range (delta + 2, 2 * delta):
                            if q0 <= log2_q0_upper:
                                for L in range(1, max_L + 1):
                                    log2_q = q0 + L * delta
                                    log2_P = log2_q / 4
                                    log2_Q = log2_q + log2_P
                                    log2_precision = math.floor(delta - math.log2(B_clean))
                                    if log2_Q < max_log_Q:
                                        big_table['lambda'].append(lambda_sec)
                                        big_table['log2_N'].append(log2_N)
                                        big_table['log2_delta'].append(delta)
                                        big_table['q0'].append(q0)
                                        big_table['L'].append(L)
                                        big_table['log2_precision'].append(log2_precision)
                                    else:
                                        break;
                                                    
                except ValueError as e:
                    print(f"Error for lambda={lambda_sec}, N={N}, h={h}: {e}")
    
    # Save the first set of results to a CSV
    bounds_df = pd.DataFrame(bounds)
    bounds_df.to_csv("bounds.csv", index=False)
    print("Bounds values saved to bounds.csv")
    
    # Save the big table to a CSV
    big_table_df = pd.DataFrame(big_table)
    big_table_df.to_csv("big_table.csv", index=False)
    print("Big table results saved to big_table.csv")

if __name__ == "__main__":
    main()