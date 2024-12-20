/*

  Inversion
  
  */

#ifndef INVERSION_ALGO_H
#define INVERSION_ALGO_H

#include <random>
#include <vector>
#include "cryptocontext.h"  // Required for CryptoContext, Ciphertext, KeyPair
#include "scheme/ckksrns/gen-cryptocontext-ckksrns.h"

using namespace lbcrypto;  // If you are using OpenFHE namespace

// Function declaration
Ciphertext<DCRTPoly> FHE_Newton_Inv(Ciphertext<DCRTPoly> b, double x_0, uint32_t max_loop, CryptoContext<DCRTPoly> cc,
                                    const PrivateKey<DCRTPoly>& secretKey);

Ciphertext<DCRTPoly> FHE_Goldschmidt_Inv(Ciphertext<DCRTPoly> CX, usint max_loop, CryptoContext<DCRTPoly> cc,
                                         const PrivateKey<DCRTPoly>& secretKey);

double ScalePlaintext(double plaintext, double& scalingFactor);

#endif
