<a name='Top'/>

koolkash-digital-cash-protocol
=======================

## Table of Contents
 1. [Authors](#Authors)
 2. [Dependencies](#Dependencies)
 3. [License](#License)
 4. [Description](#Description)
 5. [Paper](https://github.com/ToadRedCarp/koolkash-digital-cash-protocol/blob/master/HardingMcCluskey_KoolKash.pdf)

## <a name='Authors'/> Authors ([Back to Top](#Top))

 - Jonathan McCluskey (jonathan.m.mccluskey@gmail.com)
 - William Harding (wpharding1@gmail.com)

## <a name='Dependencies'/> Dependencies ([Back to Top](#Top))

Is known to work with:
- scons (v2.3.1)
- gcc (v4.9.2)
- libgmp-dev
- libboost-test-dev
- libgcrypt11-dev
- libboost-serialization-dev (v1.55.0)
- libboost-system-dev (v1.55.0)
- libboost-test-dev (v1.55.0)

Or, you can take the easy button route and build a docker container that will build and run the unit tests:
- docker build -t kool-kash .

To obtain an interactive shell to the source within the container:
- docker run -it --rm kool-kash

## <a name='License'/> License ([Back to Top](#Top))

All software contained within this project , unless noted otherwise, is
free and open source software and is licensed under the MIT License:

Copyright (c) 2015 William Harding and Jonathan McCluskey.

Permission is hereby granted, free of charge, to any person obtaining 
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

## <a name='Description'/> Description ([Back to Top](#Top))

KoolKash implements the fourth digital cash protocol in Bruce Schneier’s Applied Cryptography.  KoolKash implements the following cryptographic protocols:
- RSA
- Bit Commitment
- Secret Splitting

Enjoy!
