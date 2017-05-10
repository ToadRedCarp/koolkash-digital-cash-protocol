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

#include <sstream>
#include <thread>
#include <boost/asio.hpp>
#include <NetComm.h>

#include <cstdlib>
#include <iostream>
#include <utility>

using boost::asio::ip::tcp;

namespace
{
    const unsigned int MAX_LENGTH = 65536;
}

NetComm::NetComm::NetComm()
{
    io_service = new boost::asio::io_service();
}

NetComm::NetComm::~NetComm() 
{
    if (io_service != NULL)
    {
        delete io_service;
    }
}

std::string NetComm::NetComm::ReadAndAcknowledge(tcp::socket& sock1)
{
    char data[MAX_LENGTH];
    size_t length_sent = sock1.read_some( boost::asio::buffer(data, MAX_LENGTH) );

    std::string ret_str(data, length_sent);

    // write back an ack
    boost::asio::write( sock1, boost::asio::buffer(data, length_sent) );

    return ret_str;
}

void NetComm::NetComm::WriteAndWaitForAcknowledge( tcp::socket& sock1, std::string str )
{
    boost::asio::write( sock1, boost::asio::buffer(str.c_str(), str.size()) );

    // wait for an ack
    char ack[str.size()];
    sock1.read_some( boost::asio::buffer(ack, str.size()) );
}
        
NetComm::Server::Server( unsigned short port )
{
    acceptor = new tcp::acceptor( *io_service, tcp::endpoint( tcp::v4(), port));
}

NetComm::Server::~Server() 
{
    if (acceptor != NULL)
    {
        delete acceptor;
    }
}

void NetComm::Server::Start()
{    
    while(true)
    {
        tcp::socket sock1(*io_service);
        acceptor->accept( sock1 );
        std::thread myThread(&Server::run,this,std::move(sock1));
        myThread.join();
    }
}
            
NetComm::Client::Client( const char* host, const char* port )
    : server(host),
      serverPort(port)
{
    sock = new tcp::socket(*io_service);
    resolver = new tcp::resolver(*io_service);
}

NetComm::Client::~Client()
{
    if (sock != NULL)
    {
        delete sock;
    }
    
    if (resolver != NULL)
    {
        delete resolver;
    }
}

void NetComm::Client::Connect()
{
    boost::asio::connect(*sock, resolver->resolve({ server, serverPort } ) );
}

std::string NetComm::Client::ReadAndAcknowledge()
{
    return NetComm::NetComm::ReadAndAcknowledge(*(this->sock));
}

void NetComm::Client::WriteAndWaitForAcknowledge( std::string str )
{
    NetComm::NetComm::WriteAndWaitForAcknowledge( *(this->sock), str );
}


