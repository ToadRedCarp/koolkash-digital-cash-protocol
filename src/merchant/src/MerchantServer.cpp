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

#include "NetComm.h"
#include "MerchantServer.h"

#include "BlindSignature.h"
#include "MoneyOrder.h"
#include "MoneyOrderInfo.h"
#include "Random.h"
#include "Rsa.h"
#include "SecretSplitting.h"
#include "Utilities.h"

namespace
{
    const unsigned int BASE = 10;
}

void Merchant::MerchantServer::run(tcp::socket sock1)
{
    try
    {
        //////////////////////////////////////////////////////////////////////////////////////////
        // Connect
        ::NetComm::Client bankClient(bankHost, bankPort);
        bankClient.Connect();

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send bank open account commend
        bankClient.WriteAndWaitForAcknowledge("OPEN ACCOUNT");

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send identity and initial account amount to bank
        bankClient.WriteAndWaitForAcknowledge( identity );
        bankClient.WriteAndWaitForAcknowledge( std::to_string( 0 ) );

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send bank key commend
        bankClient.WriteAndWaitForAcknowledge("GET PUBLIC KEY");

        //////////////////////////////////////////////////////////////////////////////////////////
        // Get Bank's Key
        std::string bank_key = bankClient.ReadAndAcknowledge();
        std::string bank_mod = bankClient.ReadAndAcknowledge();
        Rsa::PublicKey pub((mpz_class(bank_mod, BASE)), (mpz_class(bank_key, BASE)));

        // Wait to get a money order for my awesome item
        std::string BuyersMoneyOrderStr = ReadAndAcknowledge(sock1);
        mpz_class BuyersMoneyOrder(BuyersMoneyOrderStr);
        mpz_class UnsignedBuyersMoneyOrder = Rsa::Unsign( BuyersMoneyOrder, pub, true );
        MoneyOrder moneyOrder;
        moneyOrder.Deserialize( Utilities::NumberToString(UnsignedBuyersMoneyOrder) );

        // Verify L's and R's on MoneyOrder
        //    - Create a string of 1's and 2's (where Left=1 and Right=2)
        //    - Write it to the Buyer
        //    - For each response
        //      -- Wait for the buyer's response
        //      -- Verify the results

        // Create the string of 1's and 2's
        std::stringstream x;
        for(unsigned int i = 0; i < moneyOrder.m_identity_strings.size(); ++i)
        {
            x << Random::GenerateRandomNumberRange(2).get_str();
        }

        // Write it to the Buyer
        WriteAndWaitForAcknowledge(sock1, x.str());

        bool verified = true;

        std::vector<std::string> buyersResponses;
        for(unsigned int i = 0; i < moneyOrder.m_identity_strings.size(); ++i)
        {
            // Wait for the buyer's response
            std::string buyersResponse = ReadAndAcknowledge( sock1 );
            buyersResponses.push_back( buyersResponse );
            CommitData c;
            c.Deserialize( buyersResponse );
            
            if (x.str()[i] == '1')
            {
                verified &= Verify(c, moneyOrder.m_identity_strings[i].first.first,
                                      moneyOrder.m_identity_strings[i].first.second);
            }
            else
            {
                verified &= Verify(c, moneyOrder.m_identity_strings[i].second.first,
                                      moneyOrder.m_identity_strings[i].second.second);
            }
        }
       
        if (verified)
        {
            bankClient.WriteAndWaitForAcknowledge("DEPOSIT MONEY ORDER");
            bankClient.WriteAndWaitForAcknowledge( identity );
            bankClient.WriteAndWaitForAcknowledge( BuyersMoneyOrderStr );
            bankClient.WriteAndWaitForAcknowledge( x.str() );
            for(unsigned int i = 0; i < moneyOrder.m_identity_strings.size(); ++i)
            {
                bankClient.WriteAndWaitForAcknowledge( buyersResponses[i] );
            }

            std::string bank_response = bankClient.ReadAndAcknowledge();
            std::cout << bank_response << std::endl;

            // if the merchant is trying to cheat, deposit money order twice
            if (cheat)
            {
                bankClient.WriteAndWaitForAcknowledge("DEPOSIT MONEY ORDER");
                bankClient.WriteAndWaitForAcknowledge( identity );
                bankClient.WriteAndWaitForAcknowledge( BuyersMoneyOrderStr );
                bankClient.WriteAndWaitForAcknowledge( x.str() );
                for(unsigned int i = 0; i < moneyOrder.m_identity_strings.size(); ++i)
                {
                    bankClient.WriteAndWaitForAcknowledge( buyersResponses[i] );
                }

                std::string bank_response = bankClient.ReadAndAcknowledge();
                std::cout << bank_response << std::endl;
            }
        }
        else
        {
            std::cout << "Buyer verification failed!!! WITCW!" << std::endl;
        }

        bankClient.WriteAndWaitForAcknowledge("CLOSE CONNECTION");
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}


