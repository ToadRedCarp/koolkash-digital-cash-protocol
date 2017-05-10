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

#include "BitCommitment.h"
#include "BlindSignature.h"
#include "MoneyOrder.h"
#include "MoneyOrderInfo.h"
#include "Random.h"
#include "Rsa.h"
#include "SecretSplitting.h"
#include "Utilities.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "NetComm.h"

namespace
{
    const unsigned int NUM_MONEY_ORDERS = 100;
    const unsigned int NUM_IDENT_STRINGS = 100;
    const unsigned int BASE = 10;
}

void BuyItem( const char* host, 
              const char* port, 
              const std::string& filename );

void GenerateMoneyOrder( const char* host, 
                         const char* port, 
                         const std::string& identity, 
                         const unsigned int amount, 
                         const std::string& filename );

void OpenAccount( const char* host, 
                  const char* port, 
                  const std::string& identity, 
                  const unsigned int amount );

int main(int argc, char* argv[])
{
    try
    {
        // commands:
        //   1) gen_money_order (identity, amount)
        //   2) buy_item
        //   3) open_account
        if (argc < 4)
        {
            std::cerr << "Usage: buyer <command> <host> <port> <filename> <identity> <amount>\n";
            return 1;
        }

        std::string cmd = argv[1];

        if (cmd == "gen_money_order")
        {
            if (argc != 7)
            {
                std::cerr << "Usage: buyer <command> <host> <port> <output_filename> <identity> <amount>\n";
                return 1;
            }

            std::string identity = argv[5];
            unsigned int amount  = std::atoi(argv[6]);
            std::string filename = argv[4];

            GenerateMoneyOrder(argv[2], argv[3], identity, amount, filename);
        }
        else if (cmd == "buy_item")
        {
            if (argc != 5)
            {
                std::cerr << "Usage: buyer <command> <host> <port> <input_filename>\n";
                return 1;
            }
            
            std::string filename = argv[4];
            BuyItem(argv[2], argv[3], filename);
        }
        else if (cmd == "open_account")
        {
            if (argc != 6)
            {
                std::cerr << "Usage: buyer <command> <host> <port> <identity> <amount>\n";
                return 1;
            }

            std::string identity = argv[4];
            unsigned int amount  = std::atoi(argv[5]);

            OpenAccount(argv[2], argv[3], identity, amount);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in Buyer::main: " << e.what() << "\n";
    }

    return 0;
}

void BuyItem( const char* host, 
              const char* port, 
              const std::string& filename )
{

    //////////////////////////////////////////////////////////////////////////////////////////
    // Connect
    NetComm::Client merchantClient(host, port);
    merchantClient.Connect();

    // Read in the signed money order
    std::stringstream filename1;
    filename1 << filename << ".bin";
    FILE* mo_info_signed_money_order = fopen(filename1.str().c_str(), "rb");
    mpz_class signed_money_order;
    mpz_inp_raw(signed_money_order.get_mpz_t(), mo_info_signed_money_order );
    fclose(mo_info_signed_money_order);

    // Read in the money order info
    std::stringstream filenameInfo;
    filenameInfo << filename << "_info.bin";
    FILE* mo_info_in2 = fopen(filenameInfo.str().c_str(), "rb");
    mpz_class in2;
    mpz_inp_raw(in2.get_mpz_t(), mo_info_in2 );
    fclose(mo_info_in2);
    MoneyOrderInfo moneyOrderInfo;
    moneyOrderInfo.Deserialize( Utilities::NumberToString(in2) );

    merchantClient.WriteAndWaitForAcknowledge( signed_money_order.get_str(BASE) );

    std::string bString = merchantClient.ReadAndAcknowledge();

    assert(bString.size() == NUM_IDENT_STRINGS);

    for(unsigned int i = 0; i < bString.size(); ++i)
    {
        if (bString[i] == '1') 
        { 
            std::string s = moneyOrderInfo.m_commit_data[i].first.Serialize();
            merchantClient.WriteAndWaitForAcknowledge(s);
        }
        else
        {
            std::string s = moneyOrderInfo.m_commit_data[i].second.Serialize();
            merchantClient.WriteAndWaitForAcknowledge(s);
        }
    }
}

void GenerateMoneyOrder( const char* host, 
                         const char* port, 
                         const std::string& identity, 
                         const unsigned int amount,
                         const std::string& filename )
{
    try
    {
        std::vector<MoneyOrder>     money_orders;
        std::vector<MoneyOrderInfo> money_orders_info;

        //////////////////////////////////////////////////////////////////////////////////////////
        // Connect
        NetComm::Client bankClient(host, port);
        bankClient.Connect();

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send bank key commend
        bankClient.WriteAndWaitForAcknowledge("GET PUBLIC KEY");

        //////////////////////////////////////////////////////////////////////////////////////////
        // Get Bank's Key
        std::string bank_key = bankClient.ReadAndAcknowledge();
        std::string bank_mod = bankClient.ReadAndAcknowledge();
        Rsa::PublicKey pub((mpz_class(bank_mod, BASE)), (mpz_class(bank_key, BASE)));

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send sign money order command
        bankClient.WriteAndWaitForAcknowledge("SIGN MONEY ORDER");

        //////////////////////////////////////////////////////////////////////////////////////////
        // Write out identity string and amount
        bankClient.WriteAndWaitForAcknowledge(identity);
        bankClient.WriteAndWaitForAcknowledge(std::to_string(amount));

        //////////////////////////////////////////////////////////////////////////////////////////
        // Tell the bank how many money orders to expect
        bankClient.WriteAndWaitForAcknowledge( std::to_string(NUM_MONEY_ORDERS) );

        //////////////////////////////////////////////////////////////////////////////////////////
        // Prepare and Write Money Orders
        mpz_class left;
        mpz_class right;

        for (unsigned int i = 0; i < NUM_MONEY_ORDERS; ++i)
        {
            MoneyOrder     ord;
            MoneyOrderInfo ord_info;

            ord.m_amount = amount;
            ord.m_uniqueness = Random::GenerateRandomNumberBits(1024).get_str();

            for (unsigned int j = 0; j < NUM_IDENT_STRINGS; ++j)
            {
                std::tie(left, right) = SecretSplitting::SplitSecret(identity);

                CommitData leftCommitData = GenCommitData(left);
                std::string leftHash = Hash( leftCommitData );

                CommitData rightCommitData = GenCommitData(right);
                std::string rightHash = Hash( rightCommitData );

                ord_info.m_commit_data.push_back(std::pair<CommitData, CommitData>(leftCommitData, rightCommitData));

                ord.m_identity_strings.push_back(
                        MoneyOrder::IdentityPair(
                            CommitPair( leftHash, leftCommitData.r1 ),
                            CommitPair( rightHash, rightCommitData.r1 )));
            }

            // remember the money order
            money_orders.push_back(ord);

            // Serialize the money order
            std::string serial_str = ord.Serialize();
            mpz_class serial_mpz = Utilities::StringToNumber(serial_str);

            // Blind the money order using the banks public key
            mpz_class blinding_factor;
            mpz_class blinded_text;
            std::tie(blinded_text, blinding_factor) = BlindSignature::Blind(serial_mpz, pub, true);
            
            // save off the blinding factor
            ord_info.m_blinding_factor = blinding_factor.get_str(BASE);
            money_orders_info.push_back(ord_info);

            // write each money order to the network
            std::string blinded_text_str = blinded_text.get_str(BASE);
            bankClient.WriteAndWaitForAcknowledge( blinded_text_str );
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        // Receive money order number that will be signed
        unsigned int mo_num = atoi(bankClient.ReadAndAcknowledge().c_str());

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send Money Order Info
        for (unsigned int i = 0; i < money_orders_info.size(); ++i)
        {
            if (i != mo_num)
            {
                bankClient.WriteAndWaitForAcknowledge( money_orders_info[i].Serialize());
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        // receive the signed money order from the bank
        std::string signed_money_order = bankClient.ReadAndAcknowledge();
        mpz_class unblinded_signed_money_order =
            BlindSignature::Unblind( mpz_class( signed_money_order, BASE),
                                     pub,
                                     mpz_class( money_orders_info[mo_num].m_blinding_factor, BASE),
                                     false );

        //////////////////////////////////////////////////////////////////////////////////////////
        // close connection with bank
        bankClient.WriteAndWaitForAcknowledge("CLOSE CONNECTION");

        std::cout << "Signed money order received and written to file" << std::endl;

        // Write out the money order to file
        std::stringstream mo_filename;
        mo_filename << filename << ".bin";
        FILE* mo_output = fopen(mo_filename.str().c_str(), "wb");
        mpz_out_raw(mo_output, unblinded_signed_money_order.get_mpz_t());
        fclose(mo_output);

        // Write out the money order info to file
        std::stringstream mo_info_filename;
        mo_info_filename << filename << "_info" << ".bin";
        FILE* mo_info_output = fopen(mo_info_filename.str().c_str(), "wb");
        mpz_out_raw(mo_info_output, Utilities::StringToNumber(money_orders_info[mo_num].Serialize()).get_mpz_t());
        fclose(mo_info_output);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void OpenAccount( const char* host, 
                  const char* port, 
                  const std::string& identity, 
                  const unsigned int amount )
{
    try
    {
        //////////////////////////////////////////////////////////////////////////////////////////
        // Connect
        NetComm::Client bankClient(host, port);
        bankClient.Connect();

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send bank open account commend
        bankClient.WriteAndWaitForAcknowledge("OPEN ACCOUNT");

        //////////////////////////////////////////////////////////////////////////////////////////
        // Send identity and initial account amount to bank
        bankClient.WriteAndWaitForAcknowledge( identity );
        bankClient.WriteAndWaitForAcknowledge( std::to_string( amount ) );

        bankClient.WriteAndWaitForAcknowledge("CLOSE CONNECTION");
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
