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

#ifndef MONEYORDERINFO_H_
#define MONEYORDERINFO_H_

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "BitCommitment.h"
#include "Serializable.h"

#include <vector>

class MoneyOrderInfo : public Serializable<MoneyOrderInfo>
{
    public:
        MoneyOrderInfo() : Serializable(this) {}
        ~MoneyOrderInfo() = default;
        
        // copy constructor
        MoneyOrderInfo(const MoneyOrderInfo& other) : Serializable(this)
        {
            m_blinding_factor = other.m_blinding_factor;
            m_commit_data     = other.m_commit_data;
        }

        std::string                                    m_blinding_factor = "";
        std::vector<std::pair<CommitData, CommitData>> m_commit_data;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & m_blinding_factor;
            ar & m_commit_data;
        }
};
#endif // MONEYORDERINFO_H_

