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

#ifndef BITCOMMITMENT_H
#define BITCOMMITMENT_H

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "Serializable.h"

#include <tuple>
#include <gmpxx.h>

typedef std::pair<std::string, std::string> CommitPair;

class CommitData : public Serializable<CommitData>
{
    public:
        CommitData() : Serializable(this) {}
        ~CommitData() = default;

        // copy constructor
        CommitData(const CommitData& other) : Serializable(this)
        {
            r1 = other.r1;
            r2 = other.r2;
            b  = other.b;
        }

        std::string r1;
        std::string r2;
        std::string b;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & r1;
            ar & r2;
            ar & b;
        }
};

CommitData  GenCommitData( mpz_class b );
std::string Hash( const CommitData& c );
bool        Verify( CommitData receivedCommitData, 
                   std::string  originalHash, 
                   std::string  originalR1 );

#endif // BITCOMMITMENT_H

