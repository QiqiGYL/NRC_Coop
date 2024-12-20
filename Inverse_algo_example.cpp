#define _USE_MATH_DEFINES

#include "benchmark/benchmark.h"
#include "math/hal/basicint.h"
#include "scheme/ckksrns/gen-cryptocontext-ckksrns.h"
#include "scheme/bfvrns/gen-cryptocontext-bfvrns.h"
#include "scheme/bgvrns/gen-cryptocontext-bgvrns.h"
#include "gen-cryptocontext.h"
#include "cryptocontext.h"
#include "encoding/random_ct.h"
#include "encoding/Inversion_algo.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>

using namespace lbcrypto;

uint32_t securityLevel = 128;
uint32_t ringDim       = 16;
uint32_t scaleModSize  = 58;
uint32_t firstModSize  = 60;
//uint32_t batchSize     = 32768;

//WARMING: too less multi_Depth will cause the program crash
uint32_t multi_Depth = 7;

int main() {
    CCParams<CryptoContextCKKSRNS> parameters;
    //parameters.SetScalingTechnique(FIXEDMANUAL);
    parameters.SetMultiplicativeDepth(multi_Depth);  //Mutiplication depth(L)
    parameters.SetScalingModSize(scaleModSize);      //Scaling Modulus(delta)
    parameters.SetFirstModSize(firstModSize);        //First Modulus(q0)
    parameters.SetRingDim(1 << ringDim);             //Ring Dimension(N)

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

    uint32_t maxBatchSize = (1 << ringDim) / 2;
    parameters.SetBatchSize(maxBatchSize);

    auto cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);

    auto keyPair = cc->KeyGen();

    cc->EvalMultKeyGen(keyPair.secretKey);

    double b_plain = 1.5;  //plaintext we use to test the three algorithm

    Plaintext b_encoded = cc->MakeCKKSPackedPlaintext(std::vector<double>{b_plain});
    auto b_ct           = cc->Encrypt(keyPair.publicKey, b_encoded);  //the b we want the inverse

    uint32_t max_loop = 5;  //max interation for Newton and

    //using Newton Inversion
    std::cout << "---------Using Newton Inversion---------" << std::endl;
    double x_0 = 0.4;  //initial guess x_0 for Newton and Goldschmidt
    try {
        auto b_inverse_Newton = FHE_Newton_Inv(b_ct, x_0, max_loop, cc, keyPair.secretKey);

        Plaintext newton_result;
        cc->Decrypt(keyPair.secretKey, b_inverse_Newton, &newton_result);
        newton_result->SetLength(1);

        std::cout << "Original plaintext value (b): " << b_plain << std::endl;
        std::cout << "Computed inverse (1/b): " << newton_result->GetCKKSPackedValue()[0]
                  << std::endl;  //with output (real part, imaginary part)
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "Error encountered outside the Newton Inversion algorithm: " << e.what() << std::endl;
    }

    //using Goldschmidt Inversion
    //WARMING: 0 < plaintext < 2
    std::cout << "---------Using Goldschmidt Inversion---------" << std::endl;
    try {
        double scalingFactor;
        bool pt_neg = (b_plain < 0);  // if pt is neg, then true
        if (b_plain < 0 || b_plain > 2) {
            std::cout << "Original plaintext does not satisfy Goldschmidt Inversion condition: " << b_plain
                      << std::endl;
            double scaledPlaintext            = ScalePlaintext(b_plain, scalingFactor);
            Plaintext scaledPlaintext_encoded = cc->MakeCKKSPackedPlaintext(std::vector<double>{scaledPlaintext});
            auto scaledCiphertext             = cc->Encrypt(keyPair.publicKey, scaledPlaintext_encoded);

            auto scaled_pt_inverse_GS = FHE_Goldschmidt_Inv(scaledCiphertext, max_loop, cc, keyPair.secretKey);

            Plaintext scaled_GS_result;
            cc->Decrypt(keyPair.secretKey, scaled_pt_inverse_GS, &scaled_GS_result);
            scaled_GS_result->SetLength(1);

            auto scaledResult = scaled_GS_result->GetCKKSPackedValue()[0];
            auto finalResult  = scaledResult / scalingFactor;
            if (pt_neg) {
                finalResult = -finalResult;
            }
            std::cout << "Original plaintext value (b): " << b_plain << std::endl;
            std::cout << "Scaled plaintext: " << scaledPlaintext << std::endl;
            std::cout << "Computed inverse (1/b) of scaled plaintext: " << scaledResult
                      << std::endl;  //with output (real part, imaginary part)
            std::cout << "Computed inverse (1/b) of original plaintext: " << finalResult
                      << std::endl;  //with output (real part, imaginary part)
        }
        else {
            auto b_inverse_GS = FHE_Goldschmidt_Inv(b_ct, max_loop, cc, keyPair.secretKey);

            Plaintext GS_result;
            cc->Decrypt(keyPair.secretKey, b_inverse_GS, &GS_result);
            GS_result->SetLength(1);

            std::cout << "Original plaintext value (b): " << b_plain << std::endl;
            std::cout << "Computed inverse (1/b): " << GS_result->GetCKKSPackedValue()[0]
                      << std::endl;  //with output (real part, imaginary part)
        }
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "Error encountered outside the Goldschmidt Inversion algorithm: " << e.what() << std::endl;
    }

    //using Chebyshev Inversion
    //WARMING: plaintext >= 1
    std::cout << "---------Using Chebyshev Inversion---------" << std::endl;
    double lowerBound = 1.0;
    double upperBound = 2.0;
    uint32_t degree   = 5;  //degree of the Chebyshev polynomial

    try {
        auto ct_Chebyshev_inverse = cc->EvalDivide(b_ct, lowerBound, upperBound, degree);

        Plaintext CS_result;
        cc->Decrypt(keyPair.secretKey, ct_Chebyshev_inverse, &CS_result);
        CS_result->SetLength(1);

        std::cout << "Original plaintext value (ct): " << b_plain << std::endl;
        std::cout << "Computed inverse (1/ct): " << CS_result->GetCKKSPackedValue()[0]
                  << std::endl;  //with output (real part, imaginary part)
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "Error encountered outside the Chebyshev Inversion algorithm: " << e.what() << std::endl;
    }

    return 0;
}