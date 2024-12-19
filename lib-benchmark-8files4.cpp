
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

// *Don't delete me*

//3. third thing I wrote, use run_benchmark_8files.sh to execute and then use merge.py to merge the files

//Generate 9 csv files with each computation benchmark time and cpu comsumption

// WARNING: parameters.SetScalingTechnique(FIXEDMANUAL) enables the user to set SetScalingModSize manually.

float lower_bound = 0.5;
float upper_bound = 1.0;

//const std::string outputFilePath = "benchmark_notime_results.csv";

struct TableRow {
    uint32_t lambda;
    uint32_t log2_N;
    uint32_t log2_delta;
    uint32_t q0;
    uint32_t L;
    uint32_t log2_precision;
    uint32_t OpenFHE_precision;
};

// Global variable to hold parsed arguments
TableRow globalRow;

// Function to parse input arguments from shell script
TableRow ParseArgs(int argc, char* argv[]) {
    if (argc < 8) {
        std::cerr << "Usage: " << argv[0]
                  << " <lambda> <log2_N> <log2_delta> <q0> <L> <log2_precision> <OpenFHE_precision>" << std::endl;
        // std::cerr << "----------------------------------" << std::endl;
        //for (int i = 0; i < argc; ++i) {
        //      std::cerr << "argv[" << i << "] = " << argv[i] << std::endl;
        //  }
        exit(-1);
    }

    TableRow row;
    row.lambda = std::stoi(argv[1]);
    //std::cerr << "securityLevel: " << row.lambda << std::endl;
    row.log2_N = std::stoi(argv[2]);
    //std::cerr << "ring dim: " << row.log2_N << std::endl;
    row.log2_delta        = std::stoi(argv[3]);
    row.q0                = std::stoi(argv[4]);
    row.L                 = std::stoi(argv[5]);
    row.log2_precision    = std::stoi(argv[6]);
    row.OpenFHE_precision = std::stoi(argv[7]);

    return row;
}

[[maybe_unused]] static CryptoContext<DCRTPoly> GenerateCKKSContext(uint32_t securityLevel, uint32_t ring_dim,
                                                                    uint32_t scaleModSize, uint32_t firstModSize,
                                                                    uint32_t multDepth) {
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
    uint32_t maxCiphertextLevel = globalRow.L;

    for (uint32_t j = 0; j <= maxCiphertextLevel; j++) {
        for (uint32_t i = 0; i <= j; i++) {
            b->Iterations(20)
                ->Unit(benchmark::kMicrosecond)
                ->Args({globalRow.lambda, globalRow.log2_N, globalRow.log2_delta, globalRow.q0, globalRow.L, j, i});
        }
    }
}

//For addPlaintext, subPlaintext of ciphertext and plaintext
void CustomArguments_Plaintext(benchmark::internal::Benchmark* b) {
    uint32_t maxCiphertextLevel = globalRow.L;

    for (uint32_t j = 0; j <= maxCiphertextLevel; j++) {
        b->Iterations(20)
            ->Unit(benchmark::kMicrosecond)
            ->Args({globalRow.lambda, globalRow.log2_N, globalRow.log2_delta, globalRow.q0, globalRow.L, j});
    }
}

//For Multiplication and Multiplication Realization of ciphertext and ciphertext
void CustomArguments_Mult_Ct(benchmark::internal::Benchmark* b) {
    uint32_t maxCiphertextLevel = globalRow.L;

    for (uint32_t j = 0; j < maxCiphertextLevel; j++) {
        for (uint32_t i = 0; i <= j; i++) {
            b->Iterations(20)
                ->Unit(benchmark::kMicrosecond)
                ->Args({globalRow.lambda, globalRow.log2_N, globalRow.log2_delta, globalRow.q0, globalRow.L, j, i});
        }
    }
}

//For Multiplication and Multiplication Realization of ciphertext and plaintext
void CustomArguments_Mult_Plaintext(benchmark::internal::Benchmark* b) {
    uint32_t maxCiphertextLevel = globalRow.L;

    for (uint32_t j = 0; j < maxCiphertextLevel; j++) {
        b->Iterations(20)
            ->Unit(benchmark::kMicrosecond)
            ->Args({globalRow.lambda, globalRow.log2_N, globalRow.log2_delta, globalRow.q0, globalRow.L, j});
    }
}

void CKKSrns_Add(benchmark::State& state) {
    uint32_t securityLevel = state.range(0);
    //std::cerr << "DEBUG: securityLevel from state.range(0): " << securityLevel << std::endl;
    // std::cerr << "DEBUG: globalRow.lambda: " << globalRow.lambda << std::endl;
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    //std::cerr << "DEBUG: Ciphertext_Level1 from state.range(5): " << Ciphertext_Level1 << std::endl;
    uint32_t Ciphertext_Level2 = state.range(6);
    //std::cerr << "DEBUG: Ciphertext_Level2 from state.range(6): " << Ciphertext_Level2 << std::endl;

    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextAdd = cc->EvalAdd(ciphertext1, ciphertext2);
        }
        // Log the results into results_data
        std::cout << "CKKSrns_Add" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << Ciphertext_Level2 << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << "\n" << std::endl;
        std::cout << "CKKSrns_Add" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

void CKKSrns_Add_plaintext(benchmark::State& state) {
    uint32_t securityLevel     = state.range(0);
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        int random_int   = rand();

        for (auto _ : state) {
            auto ciphertextAdd = cc->EvalAdd(random_int, ciphertext1);
        }
        std::cout << "CKKSrns_Add_plaintext" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << "N/A_value" << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        std::cout << "CKKSrns_Add_plaintext" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

void CKKSrns_Sub(benchmark::State& state) {
    uint32_t securityLevel     = state.range(0);
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    uint32_t Ciphertext_Level2 = state.range(6);
    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextSub = cc->EvalSub(ciphertext1, ciphertext2);
        }
        std::cout << "CKKSrns_Sub" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << Ciphertext_Level2 << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        std::cout << "CKKSrns_Sub" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

void CKKSrns_Sub_plaintext(benchmark::State& state) {
    uint32_t securityLevel     = state.range(0);
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        int random_int   = rand();

        for (auto _ : state) {
            auto ciphertextSub = cc->EvalSub(random_int, ciphertext1);
        }

        std::cout << "CKKSrns_Sub_plaintext" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << "N/A_value" << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        std::cout << "CKKSrns_Sub_plaintext" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

void CKKSrns_Mult(benchmark::State& state) {
    uint32_t securityLevel     = state.range(0);
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    uint32_t Ciphertext_Level2 = state.range(6);
    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMult(ciphertext1, ciphertext2);
        }
        std::cout << "CKKSrns_Mult" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << Ciphertext_Level2 << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        std::cout << "CKKSrns_Mult" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

void CKKSrns_Mult_plaintext(benchmark::State& state) {
    uint32_t securityLevel     = state.range(0);
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        int random_int   = rand();

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMult(random_int, ciphertext1);
        }
        std::cout << "CKKSrns_Mult_plaintext" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << "N/A_value" << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        std::cout << "CKKSrns_Mult_plaintext" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

//Note: for CKKSrns_MultNoRelin it can only do ciphertext with ciphertext
void CKKSrns_MultNoRelin(benchmark::State& state) {
    uint32_t securityLevel     = state.range(0);
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    uint32_t Ciphertext_Level2 = state.range(6);
    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMultNoRelin(ciphertext1, ciphertext2);
        }

        std::cout << "CKKSrns_MultNoRelin" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << Ciphertext_Level2 << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        std::cout << "CKKSrns_MultNoRelin" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

//Note: for CKKSrns_MultRelin it can only do ciphertext with ciphertext
void CKKSrns_MultRelin(benchmark::State& state) {
    uint32_t securityLevel     = state.range(0);
    uint32_t ring_dim          = state.range(1);
    uint32_t scaleModSize      = state.range(2);
    uint32_t firstModSize      = state.range(3);
    uint32_t multDepth         = state.range(4);
    uint32_t Ciphertext_Level1 = state.range(5);
    uint32_t Ciphertext_Level2 = state.range(6);
    try {
        CryptoContext<DCRTPoly> cc =
            GenerateCKKSContext(securityLevel, ring_dim, scaleModSize, firstModSize, multDepth);

        auto keyPair = cc->KeyGen();
        cc->EvalMultKeyGen(keyPair.secretKey);

        usint slots      = cc->GetEncodingParams()->GetBatchSize();
        auto ciphertext1 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level1);
        auto ciphertext2 = FHE_random_ct(keyPair, lower_bound, upper_bound, slots, cc, Ciphertext_Level2);

        for (auto _ : state) {
            auto ciphertextMul = cc->EvalMultAndRelinearize(ciphertext1, ciphertext2);
        }

        std::cout << "CKKSrns_MultRelin" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << Ciphertext_Level1
                  << "," << Ciphertext_Level2 << "," << globalRow.OpenFHE_precision << "\n";
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "OpenFHE Exception caught: " << e.what() << std::endl;
        std::cout << "CKKSrns_MultRelin" << "," << securityLevel << "," << ring_dim << "," << scaleModSize << ","
                  << firstModSize << "," << multDepth << "," << globalRow.log2_precision << "," << "Error" << ","
                  << "Error" << "," << globalRow.OpenFHE_precision << "\n";
    }
}

// Function to register and run benchmarks
void RunBenchmarks() {
    // Register the benchmark function
    BENCHMARK(CKKSrns_Add)->Apply(CustomArguments_Ct);
    BENCHMARK(CKKSrns_Add_plaintext)->Apply(CustomArguments_Plaintext);
    BENCHMARK(CKKSrns_Sub)->Apply(CustomArguments_Ct);
    BENCHMARK(CKKSrns_Sub_plaintext)->Apply(CustomArguments_Plaintext);
    BENCHMARK(CKKSrns_Mult)->Apply(CustomArguments_Mult_Ct);
    BENCHMARK(CKKSrns_Mult_plaintext)->Apply(CustomArguments_Mult_Plaintext);
    BENCHMARK(CKKSrns_MultNoRelin)->Apply(CustomArguments_Mult_Ct);
    BENCHMARK(CKKSrns_MultRelin)->Apply(CustomArguments_Mult_Ct);

    // Run benchmarks
    int argc    = 0;
    char** argv = nullptr;
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}

//BENCHMARK_MAIN();
int main(int argc, char* argv[]) {
    globalRow = ParseArgs(argc, argv);

    //std::cerr << "securityLevel: " << globalRow.lambda << std::endl;
    //std::cerr << "OpenFHE_precision: " << globalRow.OpenFHE_precision << std::endl;

    // Debug: Print globalRow after initialization
    //std::cerr << "DEBUG: globalRow after ParseArgs:\n";
    //std::cerr << "lambda: " << globalRow.lambda << ", log2_N: " << globalRow.log2_N
    //          << ", log2_delta: " << globalRow.log2_delta << ", q0: " << globalRow.q0 << ", L: " << globalRow.L
    //          << ", log2_precision: " << globalRow.log2_precision
    //          << ", OpenFHE_precision: " << globalRow.OpenFHE_precision << "\n";

    // Run benchmarks
    RunBenchmarks();

    // Run Google Benchmark
    //benchmark::Initialize(&argc, argv);
    //std::cout << "\nAfter benchmark::Initialize:" << std::endl;
    //std::cout << "argc: " << argc << std::endl;
    //for (int i = 0; i < argc; ++i) {
    //    std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    //}

    //benchmark::RunSpecifiedBenchmarks();

    return 0;
}
