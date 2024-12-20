#define _USE_MATH_DEFINES

#include "benchmark/benchmark.h"
#include "math/hal/basicint.h"
#include "scheme/ckksrns/gen-cryptocontext-ckksrns.h"
#include "scheme/bfvrns/gen-cryptocontext-bfvrns.h"
#include "scheme/bgvrns/gen-cryptocontext-bgvrns.h"
#include "gen-cryptocontext.h"
#include "cryptocontext.h"
#include "encoding/random_ct.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>

using namespace lbcrypto;

// This file imports db and outputs db

// This file is designed to import databse data files, where each row represents the CKKS CryptoContext parameters.
// For each row, it iterates from 0 to (cur_L - 1) for the maximum multiplication
// depth specified in the big_table.db. It multiplies two CKKS ciphertexts using EvalMult, encrypts and decrypts them,
// records the results alongside the OpenFHE precision, and outputs a CSV file.

// WARNING: For two ciphertexts multiplication, the default multiplication method is EvalMult.
// WARNING: parameters.SetScalingTechnique(FIXEDMANUAL) enables the user to set SetScalingModSize.
// WARNING: For each time runninng the code, the result OpenFHE precision could be different.

float lower_bound  = 0.5;    //lower bound for random plaintext generation
float upper_bound  = 1.0;    //upper bound for random plaintext generation
bool errorOccurred = false;  //indicate an error has occured

//generate a CryptoContext for CKKS based on input parameters
static CryptoContext<DCRTPoly> GenerateCKKSContext(uint32_t lambda, uint32_t ringDim, uint32_t scaleModSize,
                                                   uint32_t firstModSize, uint32_t multDepth) {
    CCParams<CryptoContextCKKSRNS> parameters;

    switch (lambda) {
        case 128:
            parameters.SetSecurityLevel(HEStd_128_classic);
            break;
        case 192:
            parameters.SetSecurityLevel(HEStd_192_classic);
            break;
        case 256:
            parameters.SetSecurityLevel(HEStd_256_classic);
            break;
        default:
            std::cerr << "Invalid security level! Defaulting to HEStd_NotSet." << std::endl;
            parameters.SetSecurityLevel(HEStd_NotSet);
    }
    parameters.SetRingDim(1 << ringDim);         //ring Dimension is 2^log2_N
    parameters.SetScalingModSize(scaleModSize);  //qi
    parameters.SetFirstModSize(firstModSize);    //q0
    parameters.SetMultiplicativeDepth(multDepth);

    uint32_t maxBatchSize = (1 << ringDim) / 2;
    parameters.SetBatchSize(maxBatchSize);

    auto cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    return cc;
}

int main(int argc, char* argv[]) {
    //argv[0] is the program name, the rest are the parameters
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <lambda> <log2_N> <log2_delta> <q0> <L> <calc_precision>" << std::endl;
        return -1;
    }

    uint32_t lambda         = std::stoi(argv[1]);
    uint32_t log2_N         = std::stoi(argv[2]);
    uint32_t delta          = std::stoi(argv[3]);
    uint32_t q0             = std::stoi(argv[4]);
    uint32_t cur_L          = std::stoi(argv[5]);
    uint32_t calc_precision = std::stoi(argv[6]);

    try {
        //generate CryptoContext
        auto cc      = GenerateCKKSContext(lambda, log2_N, delta, q0, cur_L);
        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);
        usint slots = cc->GetEncodingParams()->GetBatchSize();

        //nested loop for ciphertext levels
        for (uint32_t level1 = 0; level1 < cur_L; level1++) {
            for (uint32_t level2 = 0; level2 <= level1; level2++) {
                try {
                    auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, level1);
                    auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, level2);

                    auto ciphertextMul = cc->EvalMult(ciphertext1, ciphertext2);

                    Plaintext result;
                    cc->Decrypt(keyPair.secretKey, ciphertextMul, &result);
                    double logPrecision = result->GetLogPrecision();

                    //output results to standard output in CSV format
                    std::cout << level1 << "|" << level2 << "|" << logPrecision << std::endl;
                }
                catch (const std::exception& e) {
                    //print error information in the terminal and skip to the next row in the imported CSV file
                    std::cerr << "OpenFHEException:" << e.what() << "\n"
                              << "Error processing parameters: "
                              << "SecurityLevel: " << lambda << ", RingDim: " << log2_N << ", ScaleModSize: " << delta
                              << ", FirstModSize: " << q0 << ", Current_L: " << cur_L
                              << ", calculated precision: " << calc_precision << ", Ct1 level: " << level1
                              << ", Ct2 level: " << level2 << std::endl;
                    errorOccurred = true;
                    break;
                }
            }
            if (errorOccurred) {
                break;
            }
        }
    }
    catch (const std::exception& e) {
        //handle exceptions during CryptoContext setup
        std::cerr << "OpenFHEException:" << e.what() << "\n"
                  << "Error processing parameters: "
                  << "SecurityLevel: " << lambda << " RingDim: " << log2_N << " ScaleModSize: " << delta
                  << " FirstModSize: " << q0 << ", Current_L: " << cur_L << " calculated precision: " << calc_precision
                  << std::endl;
    }
    return 0;
}
