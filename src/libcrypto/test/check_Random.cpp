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

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <gmpxx.h>

#include "Random.h"
 
BOOST_AUTO_TEST_CASE(random_test_1)
{
    int num_bits = 256;
    mpz_class random_1 = Random::GenerateRandomNumberBits(num_bits);
    //gmp_printf("Random Number: %Zd\n", random_1.get_mpz_t());

    mpz_class random_2 = Random::GenerateRandomNumberBits(num_bits);
    //gmp_printf("Random_Number: %Zd\n", random_2.get_mpz_t());

    // verify that the splits, then rejoining to produce the original output
    BOOST_CHECK(random_1 != random_2);
    BOOST_CHECK_EQUAL(mpz_sizeinbase(random_1.get_mpz_t(), 2), num_bits);
    BOOST_CHECK_EQUAL(mpz_sizeinbase(random_2.get_mpz_t(), 2), num_bits);

}
