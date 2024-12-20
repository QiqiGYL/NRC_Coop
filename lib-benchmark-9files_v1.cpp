
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

using namespace lbcrypto;

//Generate 9 csv files with each computation benchmark time and cpu comsumption

// WARNING: parameters.SetScalingTechnique(FIXEDMANUAL) enables the user to set SetScalingModSize manually.
std::vector<uint32_t> ring_dims     = {16};
std::vector<uint32_t> scaleModSizes = {54};  //qi/delta
std::vector<uint32_t> firstModSizes = {60};  //q0
uint32_t securityLevel              = 128;
uint32_t multi_Depth                = 6;
uint32_t Ct_Level                   = 6;
float lower_bound                   = 0.5;
float upper_bound                   = 1.0;

const std::string outputFilePath = "benchmark_notime_results.csv";

// Data collection structure
std::map<std::string, std::vector<std::string>> results_data;

void write_results_to_csv(const std::string& outputFilePath) {
    std::ofstream outFile(outputFilePath);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open CSV file for writing at " << outputFilePath << std::endl;
        return;
    }
    std::cout << "CSV file successfully opened at " << outputFilePath << std::endl;

    std::vector<std::string> columns = {"Function Name",           "security level",         "Ring Dimension",
                                        "Scaling Modulus",         "First Modulus",          "Depth",
                                        "Ciphertext level of Ct1", "Ciphertext level of Ct2"};

    for (const auto& col : columns) {
        outFile << col << ",";
    }
    outFile << "\n";

    size_t numRows = results_data["Function Name"].size();
    for (size_t i = 0; i < numRows; i++) {
        for (const auto& col : columns) {
            outFile << results_data[col][i] << ",";
        }
        outFile << "\n";
    }

    outFile.close();
}

[[maybe_unused]] static CryptoContext<DCRTPoly> GenerateCKKSContext(uint32_t ring_dim, uint32_t scaleModSize,
                                                                    uint32_t firstModSize, uint32_t multDepth) {
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetScalingTechnique(FIXEDMANUAL);
    parameters.SetMultiplicativeDepth(multDepth);  //Mutiplication depth(L)
    parameters.SetScalingModSize(scaleModSize);    //Scaling Modulus(delta)
    parameters.SetFirstModSize(firstModSize);      //First Modulus(q0)
    parameters.SetRingDim(1 << ring_dim);          //Ring Dimension(N)

    switch (securityLevel) {
        case 128:
            parameters.SetSecurityLevel(HEStd_128_classic);
            break;
        case 256:
            parameters.SetSecurityLevel(HEStd_256_classic);
            break;
        case 192:
            parameters.SetSecurityLevel(HEStd_192_classic);
            break;
        default:
            std::cerr << "Invalid security level! Defaulting to HEStd_NotSet." << std::endl;
            parameters.SetSecurityLevel(HEStd_NotSet);
    }

    //parameters.SetBatchSize(batchSize);

    uint32_t maxBatchSize = (1 << ring_dim) / 2;
    parameters.SetBatchSize(maxBatchSize);

    auto cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    return cc;
}

//For addition, subtraction of ciphertext and ciphertext
void CustomArguments_Ct(benchmark::internal::Benchmark* b) {
    uint32_t multDepth          = multi_Depth;
    uint32_t maxCiphertextLevel = Ct_Level;

    for (uint32_t ring_dim : ring_dims) {
        for (uint32_t scaleModSize : scaleModSizes) {
            for (uint32_t firstModSize : firstModSizes) {
                for (uint32_t j = 0; j <= maxCiphertextLevel; j++) {
                    for (uint32_t i = 0; i <= j; i++) {
                        b->Iterations(20)
                            ->Unit(benchmark::kMicrosecond)
                            ->Args({ring_dim, scaleModSize, firstModSize, multDepth, j, i});
                    }
                }
            }
        }
    }
}

//For addPlaintext, subPlaintext of ciphertext and plaintext
void CustomArguments_Plaintext(benchmark::internal::Benchmark* b) {
    uint32_t multDepth          = multi_Depth;
    uint32_t maxCiphertextLevel = Ct_Level;

    for (uint32_t ring_dim : ring_dims) {
        for (uint32_t scaleModSize : scaleModSizes) {
            for (uint32_t firstModSize : firstModSizes) {
                for (uint32_t j = 0; j <= maxCiphertextLevel; j++) {
                    b->Iterations(20)
                        ->Unit(benchmark::kMicrosecond)
                        ->Args({ring_dim, scaleModSize, firstModSize, multDepth, j});
                }
            }
        }
    }
}

//For Multiplication and Multiplication Realization of ciphertext and ciphertext
void CustomArguments_Mult_Ct(benchmark::internal::Benchmark* b) {
    uint32_t multDepth          = multi_Depth;
    uint32_t maxCiphertextLevel = Ct_Level;

    for (uint32_t ring_dim : ring_dims) {
        for (uint32_t scaleModSize : scaleModSizes) {
            for (uint32_t firstModSize : firstModSizes) {
                for (uint32_t j = 0; j < maxCiphertextLevel; j++) {
                    for (uint32_t i = 0; i <= j; i++) {
                        b->Iterations(20)
                            ->Unit(benchmark::kMicrosecond)
                            ->Args({ring_dim, scaleModSize, firstModSize, multDepth, j, i});
                    }
                }
            }
        }
    }
}

//For Multiplication and Multiplication Realization of ciphertext and plaintext
void CustomArguments_Mult_Plaintext(benchmark::internal::Benchmark* b) {
    uint32_t multDepth          = multi_Depth;
    uint32_t maxCiphertextLevel = Ct_Level;

    for (uint32_t ring_dim : ring_dims) {
        for (uint32_t scaleModSize : scaleModSizes) {
            for (uint32_t firstModSize : firstModSizes) {
                for (uint32_t j = 0; j < maxCiphertextLevel; j++) {
                    b->Iterations(20)
                        ->Unit(benchmark::kMicrosecond)
                        ->Args({ring_dim, scaleModSize, firstModSize, multDepth, j});
                }
            }
        }
    }
}

void CKKSrns_Add(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    uint32_t Ciphertext_Level2 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextAdd = cc->EvalAdd(ciphertext1, ciphertext2);
        }
        // Log the results into results_data
        results_data["Function Name"].push_back("CKKSrns_Add");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back(std::to_string(Ciphertext_Level2));
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Add");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Add");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Add");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_Add)->Apply(CustomArguments_Ct);

void CKKSrns_Add_plaintext(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        int random_int   = rand();

        for (auto _ : state) {
            auto ciphertextAdd = cc->EvalAdd(random_int, ciphertext1);
        }

        results_data["Function Name"].push_back("CKKSrns_Add_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back("N/A_value");  // Not applicable
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Add_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Add_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Add_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_Add_plaintext)->Apply(CustomArguments_Plaintext);

void CKKSrns_Sub(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    uint32_t Ciphertext_Level2 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextSub = cc->EvalSub(ciphertext1, ciphertext2);
        }

        results_data["Function Name"].push_back("CKKSrns_Sub");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back(std::to_string(Ciphertext_Level2));
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Sub");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Sub");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Sub");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_Sub)->Apply(CustomArguments_Ct);

void CKKSrns_Sub_plaintext(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        int random_int   = rand();

        for (auto _ : state) {
            auto ciphertextSub = cc->EvalSub(random_int, ciphertext1);
        }

        results_data["Function Name"].push_back("CKKSrns_Sub_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back("N/A_value");  // Not applicable
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Sub_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Sub_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Sub_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_Sub_plaintext)->Apply(CustomArguments_Plaintext);

void CKKSrns_Mult(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    uint32_t Ciphertext_Level2 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMult(ciphertext1, ciphertext2);
        }

        results_data["Function Name"].push_back("CKKSrns_Mult");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back(std::to_string(Ciphertext_Level2));
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Mult");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Mult");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Mult");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_Mult)->Apply(CustomArguments_Mult_Ct);

void CKKSrns_Mult_plaintext(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        int random_int   = rand();

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMult(random_int, ciphertext1);
        }

        results_data["Function Name"].push_back("CKKSrns_Mult_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back("N/A_value");  // Not applicable
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Mult_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Mult_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_Mult_plaintext");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_Mult_plaintext)->Apply(CustomArguments_Mult_Plaintext);

//Note: for CKKSrns_MultNoRelin it can only do ciphertext with ciphertext
void CKKSrns_MultNoRelin(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    uint32_t Ciphertext_Level2 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMultNoRelin(ciphertext1, ciphertext2);
        }

        results_data["Function Name"].push_back("CKKSrns_MultNoRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back(std::to_string(Ciphertext_Level2));
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_MultNoRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_MultNoRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_MultNoRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_MultNoRelin)->Apply(CustomArguments_Mult_Ct);

//Note: for CKKSrns_MultRelin it can only do ciphertext with ciphertext
void CKKSrns_MultRelin(benchmark::State& state) {
    uint32_t ring_dim          = state.range(0);
    uint32_t scaleModSize      = state.range(1);
    uint32_t firstModSize      = state.range(2);
    uint32_t multDepth         = state.range(3);
    uint32_t Ciphertext_Level1 = state.range(4);
    uint32_t Ciphertext_Level2 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc = GenerateCKKSContext(ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMultAndRelinearize(ciphertext1, ciphertext2);
        }

        results_data["Function Name"].push_back("CKKSrns_MultRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back(std::to_string(Ciphertext_Level1));
        results_data["Ciphertext level of Ct2"].push_back(std::to_string(Ciphertext_Level2));
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_MultRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    /*
    catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        results_data["Function Name"].push_back("CKKSrns_MultRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        results_data["Function Name"].push_back("CKKSrns_MultRelin");
        results_data["security level"].push_back("128");
        results_data["Ring Dimension"].push_back(std::to_string(ring_dim));
        results_data["Scaling Modulus"].push_back(std::to_string(scaleModSize));
        results_data["First Modulus"].push_back(std::to_string(firstModSize));
        results_data["Depth"].push_back(std::to_string(multDepth));
        results_data["Ciphertext level of Ct1"].push_back("Error");
        results_data["Ciphertext level of Ct2"].push_back("Error");
    }
    */
}

BENCHMARK(CKKSrns_MultRelin)->Apply(CustomArguments_Mult_Ct);

BENCHMARK_MAIN();

// After all benchmarks finish, write the data to a CSV file
struct WriteResultsToCSV {
    ~WriteResultsToCSV() {
        write_results_to_csv(outputFilePath);
    }
} writeResultsToCSV;
