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

#include "BitCommitment.h"
#include <gcrypt.h>
#include <gmpxx.h>
#include <iostream>
#include "Random.h"
#include <tuple>
#include "Utilities.h"

CommitData GenCommitData( mpz_class b )
{
    CommitData c;
    c.r1 = Utilities::NumberToString(
            Random::GenerateRandomNumberRange(b*2));
    c.r2 = Utilities::NumberToString(
            Random::GenerateRandomNumberRange(b*2));
    c.b  = Utilities::NumberToString(b); 
    return c;
}

std::string Hash(const CommitData& c )
{
    const int size = gcry_md_get_algo_dlen(GCRY_MD_SHA256);
    char  digest[size+1];

    if (!gcry_check_version(GCRYPT_VERSION) )
        exit(-1);

    gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
            
    // Concatenate r1, r2, and b 
    std::string convertedStr = c.r1 + c.r2 + c.b;

    // hash the buffer into the digest
    gcry_md_hash_buffer( GCRY_MD_SHA256, 
                         reinterpret_cast<void*>(digest),
                         convertedStr.c_str(),
                         convertedStr.size() );
    
    // return the mpz_class version of digest
    digest[size] = '\0';
    return( std::string(digest) );
}

bool Verify( CommitData  receivedCommitData,
             std::string originalHash,
             std::string originalR1 )
{

    std::string u = Hash( receivedCommitData );
    std::string v = originalHash;
    std::string w = receivedCommitData.r1;
    std::string x = originalR1; 
   
    if ( (u.compare(v) == 0) && (w.compare(x) == 0) )
    {
        return true;
    }
    
    return false;
}

