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

#ifndef MONEYORDER_H_
#define MONEYORDER_H_

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "BitCommitment.h"
#include "Serializable.h"

#include <vector>


class MoneyOrder : public Serializable<MoneyOrder>
{
    public:
        MoneyOrder() : Serializable(this) {}
        ~MoneyOrder() = default;

        // copy constructor
        MoneyOrder(const MoneyOrder& other) : Serializable(this)
        {
            m_identity_strings = other.m_identity_strings;
            m_uniqueness       = other.m_uniqueness;
            m_amount           = other.m_amount;
        }

        typedef std::pair<CommitPair, CommitPair> IdentityPair;

        std::vector<IdentityPair> m_identity_strings;
        std::string m_uniqueness;
        unsigned int m_amount = 0;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & m_amount;
            ar & m_uniqueness;
            ar & m_identity_strings;
        }
};
#endif // MONEYORDER_H_

