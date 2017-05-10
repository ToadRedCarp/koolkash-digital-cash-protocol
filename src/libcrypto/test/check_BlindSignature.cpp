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
#include <memory>
#include <sstream>

#include "BlindSignature.h"
#include "Rsa.h"
#include "Utilities.h"

#include <omp.h>

BOOST_AUTO_TEST_CASE(blind_signature_test_1)
{
    Rsa::PrivateKey priv;
    Rsa::PublicKey  pub;
    std::tie(priv, pub) = Rsa::GenerateKeys(512);

    std::stringstream message;
    message << "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message";
    mpz_class plain_text = Utilities::StringToNumber(message.str());

    mpz_class blinding_factor;
    mpz_class blinded_text;
    double start = omp_get_wtime();
    std::tie(blinded_text, blinding_factor) = BlindSignature::Blind(plain_text, pub, true);
    double end = omp_get_wtime();
    std::cout << "Blind Signature Test 1 Timing " << end - start << "s" << std::endl;

    BOOST_CHECK(plain_text != blinded_text);

    // blinded_text = m*(k^e) mod N
    mpz_class signed_blinded_text = Rsa::Sign(blinded_text, priv, pub, false);

    BOOST_CHECK(blinded_text != signed_blinded_text);

    // signed_blinded_tex = (m^d)*k mod N
    start = omp_get_wtime();
    mpz_class unblinded_signed_text = BlindSignature::Unblind(signed_blinded_text, pub, blinding_factor, false);
    end = omp_get_wtime();
    std::cout << "Unblind Signature Test 1 Timing " << end - start << "s" << std::endl;

    BOOST_CHECK(plain_text != unblinded_signed_text);
    
    // unblinded_signed_text = (m^d) mod N
    mpz_class unsigned_plain_text = Rsa::Unsign(unblinded_signed_text, pub, true);
    std::string unblinded_str = Utilities::NumberToString(unsigned_plain_text);

    BOOST_CHECK_EQUAL(message.str(), unblinded_str);
}

