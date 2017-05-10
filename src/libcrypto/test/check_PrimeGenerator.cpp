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
#include <omp.h>

#include "PrimeGenerator.h"
 
BOOST_AUTO_TEST_CASE(prime_test_1)
{
    double start = omp_get_wtime();
    mpz_class prime = PrimeGenerator::GetPrimeNumber(512);
    double end = omp_get_wtime();
    std::cout << "Prime Generator (512 bit) Test 1 Timing " << end - start << "s" << std::endl;

    // Documentation says that 25 tests will allow a
    // composite to be returned as prime with proba-
    // bility less than 2^(-50)
    int probably_prime = mpz_probab_prime_p(prime.get_mpz_t(), 25);

    BOOST_CHECK_EQUAL(probably_prime, 1);

    start = omp_get_wtime();
    prime = PrimeGenerator::GetPrimeNumber(1024);
    end = omp_get_wtime();
    std::cout << "Prime Generator (1024 bit) Test 1 Timing " << end - start << "s" << std::endl;

    probably_prime = mpz_probab_prime_p(prime.get_mpz_t(), 25);

    BOOST_CHECK_EQUAL(probably_prime, 1);

    start = omp_get_wtime();
    prime = PrimeGenerator::GetPrimeNumber(1024);
    end = omp_get_wtime();
    std::cout << "Prime Generator (2048 bit) Test 1 Timing " << end - start << "s" << std::endl;

    probably_prime = mpz_probab_prime_p(prime.get_mpz_t(), 25);

    BOOST_CHECK_EQUAL(probably_prime, 1);
}
