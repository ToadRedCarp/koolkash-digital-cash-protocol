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

#ifndef NETCOMM_H
#define NETCOMM_H

#include <sstream>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace NetComm
{
    class NetComm
    {
        public:
            NetComm();
            ~NetComm();

            std::string ReadAndAcknowledge(tcp::socket& sock1);
            void        WriteAndWaitForAcknowledge( tcp::socket& sock1, std::string str );
        
        protected:
            boost::asio::io_service* io_service;

    };

    class Server : public NetComm
    {
        public:
            Server( unsigned short port );
            ~Server();

            void Start();
            virtual void run(tcp::socket sock1) = 0;
            
        protected:
            tcp::acceptor* acceptor;
    };

    class Client : public NetComm
    {   
        public:
            Client( const char* host, const char* port );
            ~Client();
            void Connect();
            std::string ReadAndAcknowledge();
            void        WriteAndWaitForAcknowledge( std::string str );

        protected: 
            tcp::socket* sock;
            tcp::resolver* resolver;
            const char*    server;
            const char*    serverPort;
    };

}

#endif // NETCOMM_H

