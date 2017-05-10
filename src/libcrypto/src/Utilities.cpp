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

#include "Utilities.h"

#include <cmath>
#include <iostream>
#include <tuple>

mpz_class Utilities::StringToNumber(const std::string& str)
{
    mpz_class z;
    mpz_import(z.get_mpz_t(),
               str.size(),  // if it were a "normal" string everytime, we would add 1 here for null terminate.
                            // but some of our strings will be serialization of objects... so we will compensate
                            // for null termination in StringToNumber and NumberToString
               1,  // MSW first
               sizeof(str.c_str()[0]),
               1,  // big endian
               0,  // use the full word
               str.c_str());

    return z;
}

std::string Utilities::NumberToString(const mpz_class& num)
{
    if ((mpz_sizeinbase(num.get_mpz_t(), 2) / 8.0) < 1.0)
    {
        return "";
    }

    size_t expected_num_chars = std::ceil(mpz_sizeinbase(num.get_mpz_t(), 2) / 8.0);

    char str[expected_num_chars];
    size_t num_chars;

    mpz_export(str,
               &num_chars,
               1,  // MSW first
               1,  // size = 1 byte
               1,  // big endian
               0,  // use the full word
               num.get_mpz_t());

    // We need to copy the char array this way in case there is no null terminator
    // Reference: http://stackoverflow.com/questions/4691608/copying-non-null-terminated-unsigned-char-array-to-stdstring
    std::string ret_str(str, str + sizeof(str)/sizeof(str[0]));
    return ret_str;
}

mpz_class Utilities::FastExp(const mpz_class& a,
                             const mpz_class& n,
                             const mpz_class& m)
{
    mpz_class const_one("1");
    mpz_class const_two("2");

    mpz_class e = n;
    mpz_class y("1");
    mpz_class z = a;

    while(true)
    {
        // r = e % 2
        mpz_class r;
        mpz_mod(r.get_mpz_t(), e.get_mpz_t(), const_two.get_mpz_t());

        // if e is odd
        if (r != 0)
        {
            y = y*z;

            // reduce modulo m
            mpz_mod(y.get_mpz_t(), y.get_mpz_t(), m.get_mpz_t());
        }

        // if e is less than or equal to 1
        if (e <= const_one)
        {
            return (y);
        }

        z = z*z;

        // reduce modulo m
        mpz_mod(z.get_mpz_t(), z.get_mpz_t(), m.get_mpz_t());

        // e = floor(e/2)
        mpz_fdiv_q(e.get_mpz_t(), e.get_mpz_t(), const_two.get_mpz_t());
    }
}

std::tuple<mpz_class, mpz_class, mpz_class> Utilities::ExtendedGcd(const mpz_class& a,
                                                                   const mpz_class& b)
{
    mpz_class x("1");
    mpz_class y("0");
    mpz_class g = a;
    mpz_class r("0");
    mpz_class s("1");
    mpz_class t = b;
    while (t > 0)
    {
        mpz_class q;
        mpz_fdiv_q(q.get_mpz_t(), g.get_mpz_t(), t.get_mpz_t());

        mpz_class u = x - q*r;
        mpz_class v = y - q*s;
        mpz_class w = g - q*t;
        x = r; y = s; g = t;
        r = u; s = v; t = w;
    }

    return std::make_tuple(g, x % b, y % a);
}
