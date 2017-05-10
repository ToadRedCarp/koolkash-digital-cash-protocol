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
#include <iostream>

#include "Utilities.h"
 
BOOST_AUTO_TEST_CASE(utilties_test_1)
{
    std::string str = "Bob";

    mpz_class num = Utilities::StringToNumber(str);

    // there is an additional null terminator (that is why 0x00 on end)
    mpz_class expected_num("426F62", 16);
    BOOST_CHECK(expected_num == num);
}

BOOST_AUTO_TEST_CASE(utilties_test_2)
{
    std::string expected_str = "Bob";
    mpz_class num("426F62", 16);

    std::string str = Utilities::NumberToString(num);
    BOOST_CHECK_EQUAL(str, expected_str);
}

BOOST_AUTO_TEST_CASE(utilties_test_3)
{
    mpz_class ans = Utilities::FastExp(11, 13, 53);
    BOOST_CHECK(ans == 52);
}

BOOST_AUTO_TEST_CASE(utilties_test_4)
{
    mpz_class gcd;
    mpz_class x;
    mpz_class y;

    std::tie(gcd, x, y) = Utilities::ExtendedGcd(65, 40);
    BOOST_CHECK(gcd == 5);
    BOOST_CHECK(x == -3);
    BOOST_CHECK(y == 5);

    std::tie(gcd, x, y) = Utilities::ExtendedGcd(1239, 735);
    BOOST_CHECK(gcd == 21);
    BOOST_CHECK(x == -16);
    BOOST_CHECK(y == 27);
}
