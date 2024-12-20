import math

"""
This file will contains all important function for computing CKKS parameters
"""

sigma = 3.19       # Standard deviation σ

# Task 1: Generate the upper Bound for Q 
def compute_upper_bound_Q(lambda_sec, N):
    """
    Input: Security level lambda_sec and Ring Dimension N=2^n
    Output: Upper bound of the ciphertext modulus size log2(Q)
    based on the inequality log2(Q) < (7.2 * N) / (lambda + 110) + log2(sigma)
    (See **We need to add the paper reference if possible**)
    """
    return math.floor((7.2 * N)/(lambda_sec + 110) + math.log2(sigma))

# Task 2: Generate corresponding upper bounds for log2(P) and log2(q)
def compute_log_P_q(log_Q):
    """
    Input: The ciphertext modulus size log2(Q) wit Q=Pq 

    Output: log2(q) and log2(P) with log2(q)=(4*log2(Q))/5 and log2(P)=log2(Q)/5
    """
    log_q = (4 * log_Q) / 5  
    log_P = log_Q / 5 
    return math.floor(log_q), math.floor(log_P)

# Task 3: Compute the values of B_clean
def compute_B_clean(N, h):
    """
    Input: Ring Dimension N=2^n and Hamming weight
    
    Output:
    B_clean = 8 * sigma * N * sqrt(2) + 6 * sigma * sqrt(N) + 16 * sigma * sqrt(h * N)
    """
    return 8 * sigma * N * math.sqrt(2) + 6 * math.sqrt(N) + 16 * math.sqrt(h * N)

# Task 4: Compute the lower bound of log2(delta) for each pair (N, h)
def log2_delta_bound_1(N, h):
    """
    Input: Ring Dimension N=2^n and Hamming weight
    
    Output:Lower bound of log2(delta) based on the inequality log2(delta) > log2(N + 2 * B_clean)."""
    B_clean = compute_B_clean(N, h)
    return math.ceil(math.log2(N + 2 * B_clean))
        
# Find the maximum L and log2(p)
def find_maxL_log2p(log2_qi, lambda_sec, N, B_clean):
    """
    Input: 
        - log2_qi: Current scaling modulus (delta)
        - lambda_sec: Security level parameter 
        - N: Ring dimension (N = 2^n)
        - B_clean: Noise-related term
    
    Output:
        - A dictionary containing:
            - L: Maximum multiplicative depth
            - log2_q: Computed log2 of the scaling factor q
            - log2_P: Auxiliary modulus
            - log2_Q: Ciphertext modulus size log2(Q)
            - log2_qi: Current scaling modulus (delta)
            - log2_percision: Computed as floor(log2_qi - log2(B_clean))"""
    log2_q0 = 60
    L = 1
    
    while True:
        log2_q = log2_q0 + L * log2_qi
        log2_P = log2_q / 4
        log2_Q = log2_q + log2_P

        if (lambda_sec + 110) * (log2_Q - math.log2(sigma)) / 7.2 >= N:
                L -= 1
                log2_q = log2_q0 + L * log2_qi
                log2_P = log2_q / 4
                log2_Q = log2_q + log2_P
                log2_precision = math.floor(log2_qi - math.log2(B_clean))
                return {"cur_L": L, "log2_q": log2_q, "log2_P": log2_P, 
                        "log2_Q": log2_Q, "cur_log2_delta": log2_qi, "log2_precision": log2_precision}
        
        L += 1