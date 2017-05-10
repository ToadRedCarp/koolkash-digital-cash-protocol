// The MIT License (MIT)
// 
// Copyright (c) 2015 Jonathan McCluskey and William Harding
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 

#ifndef RSA_H
#define RSA_H

#include <gmpxx.h>
#include <memory>
#include <string>
#include <tuple>

namespace Rsa
{
    typedef mpz_class PrivateKey;

    struct PublicKey
    {
        PublicKey(){}
        PublicKey(const mpz_class& moduli, const mpz_class& exponent)
            : N(moduli), e(exponent) {}

        mpz_class N;
        mpz_class e;
    };

    mpz_class Encipher(const mpz_class& plain_text,
                       const PublicKey& public_key,
                       const bool       pad);
    mpz_class Decipher(const mpz_class&  cipher_text,
                       const PrivateKey& private_key,
                       const PublicKey&  public_key,
                       const bool        pad);
    mpz_class Sign(const mpz_class&  plain_text,
                   const PrivateKey& private_key,
                   const PublicKey&  public_key,
                   const bool        pad);
    mpz_class Unsign(const mpz_class& cipher_text,
                     const PublicKey& public_key,
                     const bool       pad);

    std::tuple<PrivateKey, PublicKey> GenerateKeys(int num_bits);
};

#endif // RSA_H
