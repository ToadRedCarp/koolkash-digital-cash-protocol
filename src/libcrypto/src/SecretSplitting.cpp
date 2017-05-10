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

#include "SecretSplitting.h"
#include "Random.h"
#include "Utilities.h"

#include <cmath>
#include <iostream>

namespace
{
    const size_t MAX_CHARACTERS = 100;
}

std::tuple<mpz_class, mpz_class> SecretSplitting::SplitSecret(const std::string& secret)
{
    std::string secret_to_split = secret;

    if (secret_to_split.size() > MAX_CHARACTERS)
    {
        secret_to_split.erase(secret_to_split.begin() + MAX_CHARACTERS, secret_to_split.end());
        std::cerr << "SplitSecret(): Too many characters. Chopping off the end!" << std::endl;
    }

    mpz_class secret_to_split_z = Utilities::StringToNumber(secret_to_split);

    // get a random number
    mpz_class random_z = Random::GenerateRandomNumberBits((secret_to_split.size() + 1)*8);

    // perform XOR
    mpz_class split_secret_z    = secret_to_split_z ^ random_z;

    return std::make_tuple(split_secret_z, random_z);
}

std::string SecretSplitting::GetSecret(const mpz_class& left, const mpz_class& right)
{
    // XOR to obtain original secret number
    mpz_class secret_z = left ^ right;

    return Utilities::NumberToString(secret_z);
}
