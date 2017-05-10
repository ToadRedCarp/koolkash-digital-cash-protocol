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
#include "BankServer.h"

#include "BlindSignature.h"
#include "MoneyOrder.h"
#include "MoneyOrderInfo.h"
#include "Random.h"
#include "Rsa.h"
#include "SecretSplitting.h"
#include "Utilities.h"

#include <boost/progress.hpp>

namespace
{
    const unsigned int BASE = 10;
}

void Bank::BankServer::run(tcp::socket sock1)
{
    try
    {
        while (true)
        {
            // Read Command Decide Path Forward
            std::string cmd = ReadAndAcknowledge(sock1);

            if (cmd == BankCommands::DEPOSIT_MONEY_ORDER)
            {
                DepositMoneyOrder(sock1);
            }
            else if (cmd == BankCommands::SIGN_MONEY_ORDER)
            {
                SignMoneyOrder(sock1);
            }
            else if (cmd == BankCommands::GET_PUBLIC_KEY)
            {
                GetPublicKey(sock1);
            }
            else if (cmd == BankCommands::OPEN_ACCOUNT)
            {
                OpenAccount(sock1);
            }
            else if (cmd == BankCommands::CLOSE_CONNECTION)
            {
                break;
            }
            else
            {
                std::cerr << "Unknown command: " << cmd << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in Bank::BankServer::run(): " << e.what() << "\n";
    }
}

void Bank::BankServer::SignMoneyOrder(tcp::socket& sock1)
{
    try
    {
        //////////////////////////////////////////////////////////////////////////////////////////
        // Read identity string and amount
        std::string expected_ident = ReadAndAcknowledge(sock1);
        unsigned int expected_amount = std::atoi(ReadAndAcknowledge(sock1).c_str());

        //////////////////////////////////////////////////////////////////////////////////////////
        // Check how many money orders to expect
        unsigned int num_money_orders = atoi(ReadAndAcknowledge(sock1).c_str());

        //////////////////////////////////////////////////////////////////////////////////////////
        // Receive Money Orders
        std::vector<mpz_class> money_orders;
        std::cout << "Receiving " << num_money_orders << " money orders ... " << std::endl;
        boost::progress_display show_money_order_progress( num_money_orders );
        for (unsigned int i = 0; i < num_money_orders; ++i)
        {
            std::string money_order = ReadAndAcknowledge(sock1);
           
            money_orders.push_back((mpz_class(money_order, BASE)));
            ++show_money_order_progress;
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        // Choose random number to not verify
        mpz_class ran = Random::GenerateRandomNumberRange(num_money_orders) - 1;
        unsigned int r = std::atoi(ran.get_str(BASE).c_str());
        WriteAndWaitForAcknowledge(sock1, ran.get_str(BASE));

        //////////////////////////////////////////////////////////////////////////////////////////
        // Receive Money Order Info
        std::vector<MoneyOrderInfo> money_orders_info;
        for (unsigned int i = 0; i < num_money_orders; ++i)
        {
            if (i == r)
            {
                // empty info
                MoneyOrderInfo info;
                money_orders_info.push_back(info);
            }
            else
            {
                MoneyOrderInfo info;
                std::string money_order_info = ReadAndAcknowledge(sock1);
                info.Deserialize(money_order_info);
                money_orders_info.push_back(info);
            }
        }

        bool all_verified = true;
        std::cout << "Verifying " << num_money_orders-1 << " money orders ..." << std::endl;
        boost::progress_display show_verification_progress( num_money_orders-1 );
        for (unsigned int i = 0; i < num_money_orders; ++i)
        {
            // if this is not the money order to sign
            if (i != r)
            {
                mpz_class signed_blinded_text = Rsa::Sign(money_orders[i], priv, pub, false);
                mpz_class unblinded_signed_text = BlindSignature::Unblind(signed_blinded_text, pub, (mpz_class(money_orders_info[i].m_blinding_factor, BASE)), false);
                mpz_class unsigned_text = Rsa::Unsign(unblinded_signed_text, pub, true);

                MoneyOrder mo;
                std::string nts = Utilities::NumberToString(unsigned_text);
                mo.Deserialize(nts);

                unsigned int j = 0;
                bool verified = true;

                verified &= (mo.m_amount == expected_amount);

                for (const auto& cd : money_orders_info[i].m_commit_data)
                {
                    // verify left side
                    verified &= Verify(cd.first, mo.m_identity_strings[j].first.first,
                                                 mo.m_identity_strings[j].first.second);

                    // verify left side
                    verified &= Verify(cd.second, mo.m_identity_strings[j].second.first,
                                                  mo.m_identity_strings[j].second.second);

                    verified &= (expected_ident == SecretSplitting::GetSecret(Utilities::StringToNumber(cd.first.b), Utilities::StringToNumber(cd.second.b)));

                    ++j;
                }

                ++show_verification_progress;

                all_verified &= verified;
            }
        }

        // if they have all been verified then sign the one that is still blinded
        if (all_verified)
        {
            mpz_class signed_blinded_text = Rsa::Sign(money_orders[r], priv, pub, false);

            // send it back to the buyer
            WriteAndWaitForAcknowledge(sock1, signed_blinded_text.get_str(BASE));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in Bank::BankServer::SignMoneyOrder: " << e.what() << "\n";
    }
}

void Bank::BankServer::DepositMoneyOrder(tcp::socket& sock1)
{
    std::string identity = ReadAndAcknowledge(sock1);

    std::string BuyersMoneyOrderStr = ReadAndAcknowledge(sock1);
    mpz_class BuyersMoneyOrder(BuyersMoneyOrderStr);
    mpz_class UnsignedBuyersMoneyOrder = Rsa::Unsign( BuyersMoneyOrder, pub, true );

    MoneyOrder moneyOrder;
    moneyOrder.Deserialize( Utilities::NumberToString( UnsignedBuyersMoneyOrder ) );

    std::string selectorStr = ReadAndAcknowledge(sock1);

    std::vector<std::string> commitDataStrVector;
    for(unsigned int i = 0; i < moneyOrder.m_identity_strings.size(); ++i)
    {
        commitDataStrVector.push_back(ReadAndAcknowledge(sock1));
    }

    // check the database to determine whether this money order has already been deposited
    auto it = m_deposits.find(moneyOrder.m_uniqueness);
    if (it == m_deposits.end())
    {
        DepositInformation depositInfo( identity, moneyOrder, selectorStr, commitDataStrVector );
        m_deposits.insert( std::pair<std::string, DepositInformation>( moneyOrder.m_uniqueness, depositInfo ));

        WriteAndWaitForAcknowledge(sock1, "Deposit Successful!");
    }
    else
    {
        std::cout << "Deposit Unsuccesful.  Determining the perpetrator..." << std::endl;

        // if the selector string match, then the merchant cheated
        if (selectorStr == it->second.selectorStr)
        {
            std::cout << "The merchant, " << identity << ", cheated!" << std::endl;
        }
        else
        {
            // if there are different selector strings, then the buyer cheated
            std::cout << "The buyer cheated!  Possible identities ..." << std::endl;

            int i = 0;
            for (const auto& cd : it->second.identity_strings)
            {
                CommitData data1;
                data1.Deserialize(cd);

                CommitData data2;
                data2.Deserialize(commitDataStrVector[i]);

                std::cout << "Identity: " << SecretSplitting::GetSecret(Utilities::StringToNumber(data1.b),
                                                                        Utilities::StringToNumber(data2.b)) << std::endl;

                ++i;
            }
        }

        WriteAndWaitForAcknowledge(sock1, "Deposit Unsuccessful!");
    }
}

void Bank::BankServer::GetPublicKey(tcp::socket& sock1)
{
    try
    {
        std::string pub_str = pub.e.get_str(BASE);
        std::string mod_str = pub.N.get_str(BASE);

        WriteAndWaitForAcknowledge(sock1, pub_str);
        WriteAndWaitForAcknowledge(sock1, mod_str);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in Bank::BankServer::GetPublicKey(): " << e.what() << "\n";
    }
}

void Bank::BankServer::OpenAccount(tcp::socket& sock1)
{
    try
    {
        std::string identity = ReadAndAcknowledge( sock1 );
        unsigned int amount  = std::atoi( ReadAndAcknowledge( sock1 ).c_str() );

        if (m_accounts.find( identity ) == m_accounts.end())
        {
            AccountInformation ai( amount );
            m_accounts.insert( std::pair<std::string, AccountInformation>( identity, ai ) );
            std::cout << "New account opened for: " << identity << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in Bank::BankServer::OpenAccount(): " << e.what() << "\n";
    }
}

