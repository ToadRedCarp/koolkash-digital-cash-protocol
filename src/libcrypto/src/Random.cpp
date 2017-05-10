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

#include "Random.h"

#include <fstream>

mpz_class Random::GenerateRandomNumberBits(unsigned int num_bits)
{
    // Get a random seed
    uint16_t num_bytes = num_bits/4; 
    char block[num_bytes]; 
    std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary); 
    urandom.read(block, num_bytes - 1);  // subtract one for null terminater
    urandom.close();

    // turn those random bits into a large number "seed"
    mpz_class seed;
    mpz_import(seed.get_mpz_t(),
               sizeof(block),
               1,  // MSW first
               sizeof(block[0]),
               1,  // big endian
               0,  // use the full word
               block);

    // generate a larger number
    gmp_randclass rand_gen(gmp_randinit_default);
    rand_gen.seed(seed); 
    mpz_class random_z = rand_gen.get_z_bits(num_bits);

    // check whether it is indeed num_bits long
    if (mpz_sizeinbase(random_z.get_mpz_t(), 2) != num_bits)
    {
        // example: if num_bits = 100, then add 2^99 (i.e. 10000000...)
        //          to random_z in order to pop a 1 on the left side.
        random_z += (mpz_class("1") << (num_bits - 1));
    }

    return random_z;
}

mpz_class Random::GenerateRandomNumberRange(const mpz_class& num)
{
    // Get a random seed
    uint16_t num_bytes = 2; 
    char block[num_bytes]; 
    std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary); 
    urandom.read(block, num_bytes - 1);  // subtract one for null terminater
    urandom.close();

    // turn those random bits into a large number "seed"
    mpz_class seed;
    mpz_import(seed.get_mpz_t(),
               sizeof(block),
               1,  // MSW first
               sizeof(block[0]),
               1,  // big endian
               0,  // use the full word
               block);

    // generate a larger number
    gmp_randclass rand_gen(gmp_randinit_default);
    rand_gen.seed(seed); 
    mpz_class random_z = rand_gen.get_z_range(num);

    // get_z_range returns a value between 0 and n-1, so add 1 to it here
    return (random_z + 1);
}
