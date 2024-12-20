/*

  Generates random ciphertexts with desired levels

 */

#include "encoding/random_ct.h"  // Include the header file

Ciphertext<DCRTPoly> FHE_random_ct(KeyPair<DCRTPoly> keys, float lower_bound, float upper_bound, size_t batch_Size,
                                   CryptoContext<DCRTPoly> cc, size_t level) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> dis(lower_bound, upper_bound);

    std::vector<double> x;

    for (size_t i = 0; i < batch_Size; i++) {
        double tmp = dis(gen);
        x.push_back(float(tmp));
    }
    Plaintext ptxt          = cc->MakeCKKSPackedPlaintext(x);
    Ciphertext<DCRTPoly> cy = cc->Encrypt(keys.publicKey, ptxt);

    if (level == 0) {
        return cy;
    }
    else {
        //for case a:
        //Ciphertext<DCRTPoly> temp;
        //temp = cy->Clone();

        //std::cout << "----------------START-------------------" << std::endl;

        for (size_t i = 0; i < level; i++) {
            //Case a: level_0 level_i
            //cy = cc->EvalMult(cy, temp);
            //Case b: level_i level_i
            cy = cc->EvalMult(cy, cy);
            //std::cout << "Current number of levels: " << cy->GetLevel() << std::endl;
        }

        //std::cout << "-----------------END------------------" << std::endl;

        //std::cout << "Total number of levels: " << cy->GetLevel() << std::endl;

        return cy;
    }
}