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
#include <memory>
#include <gmpxx.h>
#include "Rsa.h"
#include "Utilities.h"

#include <sstream>
#include <omp.h>

namespace
{
    const unsigned int BASE = 10;
}

BOOST_AUTO_TEST_CASE(Rsa_test_simple)
{
    //p = 193
    //q = 101
    Rsa::PrivateKey priv("2743", BASE);
    Rsa::PublicKey  pub((mpz_class("19493", BASE)), (mpz_class("7", BASE)));

    std::stringstream message;
    message << "aaaaaa";
    {
        mpz_class plain_text = Utilities::StringToNumber(message.str());

        mpz_class cipher_text = Rsa::Encipher(plain_text, pub, true);
        mpz_class deciphered_text = Rsa::Decipher(cipher_text, priv, pub, true);

        BOOST_CHECK_EQUAL(Utilities::NumberToString(plain_text), Utilities::NumberToString(deciphered_text));
    }
    {
        message << "a";
        mpz_class plain_text = Utilities::StringToNumber(message.str());

        mpz_class cipher_text = Rsa::Encipher(plain_text, pub, true);
        mpz_class deciphered_text = Rsa::Decipher(cipher_text, priv, pub, true);

        BOOST_CHECK_EQUAL(Utilities::NumberToString(plain_text), Utilities::NumberToString(deciphered_text));
    }
}

BOOST_AUTO_TEST_CASE(Rsa_test_alphabet)
{
    Rsa::PrivateKey priv("2743", BASE);
    Rsa::PublicKey  pub((mpz_class("19493", BASE)), (mpz_class("7", BASE)));

    std::stringstream message;
    message << "abcdefghijklmnopqrstuvwxyz";
    mpz_class plain_text = Utilities::StringToNumber(message.str());

    mpz_class cipher_text = Rsa::Encipher(plain_text, pub, true);
    mpz_class deciphered_text = Rsa::Decipher(cipher_text, priv, pub, true);

    BOOST_CHECK_EQUAL(Utilities::NumberToString(plain_text), Utilities::NumberToString(deciphered_text));
}

BOOST_AUTO_TEST_CASE(Rsa_test_signature)
{
    Rsa::PrivateKey priv("2743", BASE);
    Rsa::PublicKey  pub((mpz_class("19493", BASE)), (mpz_class("7", BASE)));

    std::stringstream message;
    message << "abcdefghijklmnopqrstuvwxyz";
    mpz_class plain_text = Utilities::StringToNumber(message.str());

    mpz_class cipher_text = Rsa::Sign(plain_text, priv, pub, true);
    mpz_class deciphered_text = Rsa::Unsign(cipher_text, pub, true);

    BOOST_CHECK_EQUAL(Utilities::NumberToString(plain_text), Utilities::NumberToString(deciphered_text));
}
 
BOOST_AUTO_TEST_CASE(Rsa_test_1)
{
    Rsa::PrivateKey priv;
    Rsa::PublicKey  pub;
    std::tie(priv, pub) = Rsa::GenerateKeys(1024);

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
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message";
    mpz_class plain_text = Utilities::StringToNumber(message.str());

    double start = omp_get_wtime();
    mpz_class cipher_text = Rsa::Encipher(plain_text, pub, true);
    double end = omp_get_wtime();
    std::cout << "RSA Encipher (1024 bit) Test 1 Timing " << end - start << "s" << std::endl;

    start = omp_get_wtime();
    mpz_class deciphered_text = Rsa::Decipher(cipher_text, priv, pub, true);
    end = omp_get_wtime();
    std::cout << "RSA Decipher (1024 bit) Test 1 Timing " << end - start << "s" << std::endl;

    BOOST_CHECK_EQUAL(Utilities::NumberToString(plain_text), Utilities::NumberToString(deciphered_text));
}

BOOST_AUTO_TEST_CASE(Rsa_test_2)
{
    Rsa::PrivateKey priv;
    Rsa::PublicKey  pub;
    std::tie(priv, pub) = Rsa::GenerateKeys(1024);

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
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message" <<
               "this is a very secret message";

    mpz_class plain_text = Utilities::StringToNumber(message.str());

    double start = omp_get_wtime();
    mpz_class signed_text = Rsa::Sign(plain_text, priv, pub, true);
    double end = omp_get_wtime();
    std::cout << "RSA Signature (1024 bit) Test 1 Timing " << end - start << "s" << std::endl;

    start = omp_get_wtime();
    mpz_class unsigned_text = Rsa::Unsign(signed_text, pub, true);
    end = omp_get_wtime();
    std::cout << "RSA Verify (1024 bit) Test 1 Timing " << end - start << "s" << std::endl;

    BOOST_CHECK(plain_text == unsigned_text);
}

BOOST_AUTO_TEST_CASE(Rsa_test_3)
{
    Rsa::PrivateKey priv1("2743", BASE);
    Rsa::PublicKey  pub1((mpz_class("19493", BASE)), (mpz_class("7", BASE)));

    //p = 107
    //q = 139
    Rsa::PrivateKey priv2("8359", BASE);
    Rsa::PublicKey  pub2((mpz_class("14873", BASE)), (mpz_class("7", BASE)));

    mpz_class plain_text("654321");

    mpz_class signed_text = Rsa::Sign(plain_text, priv1, pub1, true);
    mpz_class signed_cipher_text = Rsa::Encipher(signed_text, pub2, false);
    mpz_class new_signed_text = Rsa::Decipher(signed_cipher_text, priv2, pub2, false);
    BOOST_CHECK(signed_text == new_signed_text);
    mpz_class unsigned_plain_text = Rsa::Unsign(new_signed_text, pub1, true);

    BOOST_CHECK(plain_text == unsigned_plain_text);
}

