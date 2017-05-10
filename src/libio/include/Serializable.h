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

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

template<class T>
class Serializable
{
    friend T;
    public:

        Serializable(T* tPtr_in)
        {
            tPtr = tPtr_in;
        }

        // Serialize Reference: http://stackoverflow.com/questions/3015582/direct-boost-serialization-to-char-array
        std::string Serialize()
        {
            std::string serial_str;
            boost::iostreams::back_insert_device<std::string> inserter(serial_str);
            boost::iostreams::stream<boost::iostreams::back_insert_device<std::string>> s(inserter);
            boost::archive::binary_oarchive oa(s);
            oa << *tPtr;

            s.flush();
            return serial_str;
        }

        void Deserialize(const std::string& serial_str)
        {
            boost::iostreams::basic_array_source<char> device(serial_str.data(), serial_str.size());
            boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
            boost::archive::binary_iarchive ia(s);
            ia >> *tPtr;
        }

    private:
        T* tPtr;

};
#endif // SERIALIZABLE_H

