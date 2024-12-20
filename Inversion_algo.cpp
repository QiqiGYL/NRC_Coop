/*

  Calculates the inversion of a given ciphertext

 */

#include "encoding/Inversion_algo.h"  // Include the header file

//Returns the inversion of the ciphertext (1/b)
Ciphertext<DCRTPoly> FHE_Newton_Inv(Ciphertext<DCRTPoly> b, double x_0, uint32_t max_loop, CryptoContext<DCRTPoly> cc,
                                    const PrivateKey<DCRTPoly>& secretKey) {
    Ciphertext<DCRTPoly> z_0;

    try {
        z_0 = cc->EvalMult(x_0, cc->EvalSub(2, cc->EvalMult(x_0, b)));
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "Error during initialization of z_0: " << e.what() << std::endl;
        return nullptr;  //null pointer to indicate failure
    }

    for (usint i = 0; i < max_loop; i++) {
        try {
            z_0 = cc->EvalMult(z_0, cc->EvalSub(2, cc->EvalMult(z_0, b)));
            //z_0 = cc->EvalMultAndRelinearize(z_0, cc->EvalSub(2, cc->EvalMult(z_0, b)));
            //z_0 = cc->ModReduce(z_0);

            //Use this to debug, can delete
            //-------------------
            Plaintext intermediate;
            cc->Decrypt(secretKey, z_0, &intermediate);
            intermediate->SetLength(1);
            std::cout << "Iteration " << i << ": Approximation = " << intermediate->GetCKKSPackedValue()[0]
                      << std::endl;
            //-------------------
        }
        catch (const lbcrypto::OpenFHEException& e) {
            std::cerr << "Iteration " << i << ": Encountered error: " << e.what() << std::endl;
            break;
        }
    }

    return z_0;
}

// Returns the inversion of the ciphertext (1/CX)
Ciphertext<DCRTPoly> FHE_Goldschmidt_Inv(Ciphertext<DCRTPoly> CX, usint max_loop, CryptoContext<DCRTPoly> cc,
                                         const PrivateKey<DCRTPoly>& secretKey) {
    Ciphertext<DCRTPoly> a;

    Ciphertext<DCRTPoly> b;

    try {
        a = cc->EvalSub(2, CX);

        b = cc->EvalSub(1, CX);
    }
    catch (const lbcrypto::OpenFHEException& e) {
        std::cerr << "Error during initialization of a and b: " << e.what() << std::endl;
        return nullptr;  //null pointer to indicate failure
    }

    for (usint i = 0; i < max_loop; i++) {
        try {
            cc->EvalSquareInPlace(b);

            a = cc->EvalMult(a, cc->EvalAdd(1, b));

            //Use this to debug, can delete
            //-------------------
            Plaintext intermediate;
            cc->Decrypt(secretKey, a, &intermediate);
            intermediate->SetLength(1);
            std::cout << "Iteration " << i << ": Approximation = " << intermediate->GetCKKSPackedValue()[0]
                      << std::endl;
            //-------------------
        }
        catch (const lbcrypto::OpenFHEException& e) {
            std::cerr << "Iteration " << i << ": Encountered error: " << e.what() << std::endl;
            break;
        }
    }
    return a;
}

// Used for FHE_Goldschmidt_Inv to scale the plaintext to be in the range 0 < plaintext < 2
double ScalePlaintext(double plaintext, double& scalingFactor) {
    scalingFactor = 1.0;

    //if plaintext is less than or equal to 0
    if (plaintext <= 0.0) {
        std::cerr << "Invalid plaintext: " << plaintext << ". Taking absolute value." << std::endl;
        plaintext = std::abs(plaintext);
    }

    //scale iteratively until 0 < plaintext < 2
    while (plaintext >= 2.0 || plaintext <= 0.0) {
        if (plaintext > 2.0) {
            plaintext /= 2.0;
            scalingFactor *= 2.0;  //keep track of the scaling factor
        }
        if (plaintext == 2.0) {
            plaintext /= 1.6;
            scalingFactor *= 1.6;  //keep track of the scaling factor
        }

        else if (plaintext <= 0.0) {
            std::cerr << "Unexpected case after adjustment: plaintext <= 0.0. Exiting loop." << std::endl;
            break;  // Should not happen after taking absolute value
        }
    }

    return plaintext;
}
