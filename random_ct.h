/*

  Generates random ciphertexts with desired levels

 */

#ifndef RANDOM_CT_H
#define RANDOM_CT_H

#include <random>
#include <vector>
#include "cryptocontext.h"  // Required for CryptoContext, Ciphertext, KeyPair
#include "scheme/ckksrns/gen-cryptocontext-ckksrns.h"

using namespace lbcrypto;  // If you are using OpenFHE namespace

// Function declaration
Ciphertext<DCRTPoly> FHE_random_ct(KeyPair<DCRTPoly> keys, float lower_bound, float upper_bound, size_t batch_Size,
                                   CryptoContext<DCRTPoly> cc, size_t level);

#endif
