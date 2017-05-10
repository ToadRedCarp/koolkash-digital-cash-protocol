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

#ifndef BANKSERVER_H
#define BANKSERVER_H

#include <cstdlib>
#include "MoneyOrder.h"
#include "NetComm.h"
#include "Rsa.h"

namespace BankCommands
{
    const std::string DEPOSIT_MONEY_ORDER  = "DEPOSIT MONEY ORDER";
    const std::string SIGN_MONEY_ORDER     = "SIGN MONEY ORDER";
    const std::string GET_PUBLIC_KEY       = "GET PUBLIC KEY";
    const std::string CLOSE_CONNECTION     = "CLOSE CONNECTION";
    const std::string OPEN_ACCOUNT         = "OPEN ACCOUNT";
}

namespace Bank
{
    class BankServer : public NetComm::Server
    {
        public:
            BankServer( char* port ) : Server( std::atoi(port) ) 
            {
                std::tie(priv, pub) = Rsa::GenerateKeys(256);
            }
            ~BankServer() = default;
            void run(tcp::socket sock1);

            struct AccountInformation
            {
                AccountInformation(const unsigned int a) : amount(a) {};
                unsigned int amount   = 0;
            };

            struct DepositInformation
            {
                DepositInformation(const std::string& depositorId, 
                                   const MoneyOrder& moneyOrder,
                                   const std::string& selectorStr,
                                   const std::vector<std::string>& ident_strings)
                    : depositorIdentity(depositorId),
                      moneyOrder(moneyOrder),
                      selectorStr(selectorStr),
                      identity_strings(ident_strings){}

                DepositInformation( const DepositInformation& other )
                    : depositorIdentity(other.depositorIdentity),
                      moneyOrder(other.moneyOrder),
                      selectorStr(other.selectorStr),
                      identity_strings(other.identity_strings){}

                std::string depositorIdentity = "";
                MoneyOrder  moneyOrder;
                std::string selectorStr = "";
                std::vector<std::string> identity_strings;
            };

        private:

            void SignMoneyOrder(tcp::socket& sock1);
            void DepositMoneyOrder(tcp::socket& sock1);
            void GetPublicKey(tcp::socket& sock1);
            void OpenAccount(tcp::socket& sock1);

            // maps  identity string to  account information
            std::map<std::string, BankServer::AccountInformation> m_accounts;
            
            // maps  uniqueness string to deposit information
            std::map<std::string, BankServer::DepositInformation> m_deposits;

            Rsa::PrivateKey priv;
            Rsa::PublicKey  pub;
    };
}

#endif // BANKSERVER_H
