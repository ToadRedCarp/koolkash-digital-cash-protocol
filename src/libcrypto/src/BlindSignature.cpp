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

#include "BlindSignature.h"
#include "Random.h"
#include "Utilities.h"

#include <gmpxx.h>
#include <iostream>

namespace
{
    const unsigned int BASE = 10;
}

std::tuple<mpz_class, mpz_class> BlindSignature::Blind(const mpz_class&      plain_text,
                                                       const Rsa::PublicKey& public_key,
                                                       const bool            pad)
{
    // get random number between 1 and N that is coprime with N
    mpz_class blinding_factor = Random::GenerateRandomNumberRange(public_key.N);
    while (true)
    {
        mpz_class gcd;
        mpz_class x;
        mpz_class y;
        std::tie(gcd, x, y) = Utilities::ExtendedGcd(public_key.N, blinding_factor);

        if (gcd == 1)
        {
            break;
        }

        blinding_factor = Random::GenerateRandomNumberRange(public_key.N);
    }

    size_t modulus_size = mpz_sizeinbase(public_key.N.get_mpz_t(), BASE);
    size_t block_size = pad ? (modulus_size - 3) : modulus_size;

    const std::string pt_str = plain_text.get_str(BASE);
    std::string bt_str = "";

    // compute k^e
    mpz_class k_e = Utilities::FastExp(blinding_factor, public_key.e, public_key.N);

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

        // calculate blinded block and reduce mod N
        mpz_class bt_block_z = block_z*k_e;
        mpz_mod(bt_block_z.get_mpz_t(), bt_block_z.get_mpz_t(), public_key.N.get_mpz_t());

        std::string bt_block_str = bt_block_z.get_str(BASE);
        while (bt_block_str.size() < modulus_size)
        {
            // prepend with 0
            bt_block_str.insert(0, 1, '0');
        }

        bt_str += bt_block_str;
    }

    // now make it one big number
    mpz_class blind_text(bt_str, BASE);

    // find mult. mod. inverse of blinding_factor
    // N*x + blind_factor*y = gcd(N, blind_factor)
    mpz_class gcd;
    mpz_class x;
    mpz_class y;
    std::tie(gcd, x, y) = Utilities::ExtendedGcd(public_key.N, blinding_factor);

    if (gcd != 1)
    {
        std::cerr << "The blinding factor is not coprime with N" << std::endl;
    }

    // reduce mod N
    mpz_mod(y.get_mpz_t(), y.get_mpz_t(), public_key.N.get_mpz_t());

    return std::make_tuple(blind_text, y);
}

mpz_class BlindSignature::Unblind(const mpz_class&      blinded_text,
                                  const Rsa::PublicKey& public_key,
                                  const mpz_class&      blind_factor,
                                  const bool            pad)
{
    size_t block_size = mpz_sizeinbase(public_key.N.get_mpz_t(), BASE);
    std::string bt_str = blinded_text.get_str(BASE);
    std::string pt_str = "";

    // we need to make sure that the blinded text coming in is padded correctly
    while ((bt_str.size() % block_size) != 0)
    {
        // prepend with 0
        bt_str.insert(0, 1, '0');
    }

    for (size_t i = 0; i < bt_str.size(); i += block_size)
    {
        std::string block_str = bt_str.substr(i, block_size);

        // calculate plain text block and reduce mod N
        mpz_class block_z(block_str, BASE);
        mpz_class pt_block_z = block_z*blind_factor;
        mpz_mod(pt_block_z.get_mpz_t(), pt_block_z.get_mpz_t(), public_key.N.get_mpz_t());

        std::string pt_block_str = pt_block_z.get_str(BASE);
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

        // append each plain text block to the overall plain text string
        pt_str += pt_block_str;
    }

    // now make it one big number
    mpz_class plain_text(pt_str, BASE);

    return plain_text;
}
