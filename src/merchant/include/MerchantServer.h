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

#ifndef MERCHANTSERVER_H
#define MERCHANTSERVER_H

#include <cstdlib>
#include "NetComm.h"

namespace Merchant
{
    class MerchantServer : public NetComm::Server
    {
        public:
            MerchantServer( const char* port,
                            const char* bankHost,
                            const char* bankPort,
                            const char* identity,
                            const bool  cheat ) 
                : Server( std::atoi(port) ),
                  bankHost( bankHost ),
                  bankPort( bankPort ),
                  identity( identity ),
                  cheat( cheat )
                  {}
            ~MerchantServer() = default;
            void run(tcp::socket sock1);

        private:
            const char* bankHost;
            const char* bankPort;
            std::string identity;
            bool  cheat;
    };
}

#endif // MERCHANTSERVER_H

