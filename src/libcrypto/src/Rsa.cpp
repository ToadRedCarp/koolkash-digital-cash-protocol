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

#include "Rsa.h"
#include "PrimeGenerator.h"
#include "Utilities.h"

#include <gmpxx.h>
#include <memory>
#include <string>
#include <iostream>

namespace
{
    const unsigned int BASE = 10;

    mpz_class EncipherSignBlocks(const mpz_class& plain_text,
                                 const mpz_class& key,
                                 const mpz_class& modulus,
                                 const bool       pad)
    {
        size_t modulus_size = mpz_sizeinbase(modulus.get_mpz_t(), BASE);
        size_t block_size = pad ? (modulus_size - 3) : modulus_size;
        std::string pt_str = plain_text.get_str(BASE);
        std::string ct_str = "";

        if (!pad)
        {
            // we need to make sure that the text coming in is a multiple of the modulus size
            // in case we've lost some zeros off of the front
            while ((pt_str.size() % modulus_size) != 0)
            {
                // prepend with 0
                pt_str.insert(0, 1, '0');
            }
        }

        for (size_t i = 0; i < pt_str.size(); i += block_size)
        {
            std::string block_str = pt_str.substr(i, block_size);

            if (pad)
            {
                // pad with 0111...0
                block_str.insert(0, 1, '0');
                while ((block_str.size() % (modulus_size - 1)) != 0)
                {
                    // prepend with 1
                    block_str.insert(0, 1, '1');
                }
                block_str.insert(0, 1, '0');
            }

            mpz_class block_z(block_str, BASE);

            // based on this paper: http://ocw.upc.edu/sites/default/files/materials/15012145/36492-3048.pdf
            // we want to add leading zeros onto our cipher text blocks to fill them out to a size of the same
            // number of digits as N.  See section 1.1.3, 1.1.4.
            std::string ct_block_str = Utilities::FastExp(block_z, key, modulus).get_str(BASE);
            while (ct_block_str.size() < modulus_size)
            {
                // prepend with 0
                ct_block_str.insert(0, 1, '0');
            }

            // append each cipher text block to the overall cipher text string
            ct_str += ct_block_str;
        }

        // now make it one big number
        mpz_class cipher_text(ct_str, BASE);

        return cipher_text;
    }

    mpz_class DecipherUnsignBlocks(const mpz_class& cipher_text,
                                   const mpz_class& key,
                                   const mpz_class& modulus,
                                   const bool       pad)
    {
        size_t block_size = mpz_sizeinbase(modulus.get_mpz_t(), BASE);
        std::string ct_str = cipher_text.get_str(BASE);

        // we need to make sure that the text coming in is padded correctly
        while ((ct_str.size() % block_size) != 0)
        {
            // prepend with 0
            ct_str.insert(0, 1, '0');
        }

        std::string pt_str = "";

        for (size_t i = 0; i < ct_str.size(); i += block_size)
        {
            std::string block_str = ct_str.substr(i, block_size);
            mpz_class block_z(block_str, BASE);

            std::string pt_block_str = Utilities::FastExp(block_z, key, modulus).get_str(BASE);

            if (pad)
            {
                //strip off the 0111...0 padding
                pt_block_str.erase(0, pt_block_str.find_first_of('0', 1) + 1);
            }
            else
            {
                // if this is not the end of the chain, then we want to pad it back out to size of N
                while (pt_block_str.size() < block_size)
                {
                    // prepend with 0
                    pt_block_str.insert(0, 1, '0');
                }
            }

            // append each cipher text block to the overall plain text string
            pt_str += pt_block_str;
        }

        // now make it one big number
        mpz_class plain_text(pt_str, BASE);

        return plain_text;
    }
}

mpz_class Rsa::Encipher(const mpz_class& plain_text,
                        const PublicKey& public_key,
                        const bool       pad)
{       
    return EncipherSignBlocks(plain_text, public_key.e, public_key.N, pad);
}

mpz_class Rsa::Decipher(const mpz_class&  cipher_text,
                        const PrivateKey& private_key,
                        const PublicKey&  public_key,
                        const bool        pad)
{
    return DecipherUnsignBlocks(cipher_text, private_key, public_key.N, pad);
}

mpz_class Rsa::Sign(const mpz_class&  plain_text,
                    const PrivateKey& private_key,
                    const PublicKey&  public_key,
                    const bool        pad)
{
    return EncipherSignBlocks(plain_text, private_key, public_key.N, pad);
}

mpz_class Rsa::Unsign(const mpz_class& cipher_text,
                      const PublicKey& public_key,
                      const bool       pad)
{
    return DecipherUnsignBlocks(cipher_text, public_key.e, public_key.N, pad);
}

std::tuple<Rsa::PrivateKey, Rsa::PublicKey> Rsa::GenerateKeys(int num_bits)
{
    mpz_class p = PrimeGenerator::GetPrimeNumber(num_bits);
    mpz_class q = PrimeGenerator::GetPrimeNumber(num_bits);

    mpz_class n = p*q;
    mpz_class phi = (p-1)*(q-1);

    //http://crypto.stackexchange.com/questions/13166/method-to-calculating-e-in-rsa
    // Use the largest known Fermat number (see wiki page and above article).
    mpz_class e = 65537;

    // phi(n)*x + e*y = gcd(phi(n), e)
    mpz_class gcd;
    mpz_class x;
    mpz_class y;
    std::tie(gcd, x, y) = Utilities::ExtendedGcd(phi, e);

    // reduce y modulo phi
    mpz_mod(y.get_mpz_t(), y.get_mpz_t(), phi.get_mpz_t());

    return std::make_tuple(Rsa::PrivateKey(y), Rsa::PublicKey(n, e));
}

