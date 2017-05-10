#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Master Test Suite
#include <boost/test/unit_test.hpp>

#include "BlindSignature.h"
#include "MoneyOrder.h"
#include "MoneyOrderInfo.h"
#include "Rsa.h"
#include "SecretSplitting.h"
#include "Utilities.h"

#include <gmpxx.h>
#include <string>
 
BOOST_AUTO_TEST_CASE(money_order_test_1)
{
    MoneyOrder old_mo;

    old_mo.m_amount = 1000;
    old_mo.m_uniqueness = "the uniqueness string";

    for (int i = 0; i < 100; ++i)
    {
        mpz_class left;
        mpz_class right;
        std::tie(left, right) = SecretSplitting::SplitSecret("this is a test");

        old_mo.m_identity_strings.push_back(
               MoneyOrder::IdentityPair(
                   CommitPair( "leftHash_0", "leftR1_0" ),
                   CommitPair( "rightHash_0", "rightR1_0" )));

        old_mo.m_identity_strings.push_back(
               MoneyOrder::IdentityPair(
                   CommitPair( "leftHash_1", "leftR1_1" ),
                   CommitPair( "rightHash_1", "rightR1_1" )));
    }

    std::string serial_str = old_mo.Serialize();
  
    // Write out the money order to file
    std::stringstream mo_filename;
    mo_filename << "x.bin";
    FILE* mo_output = fopen(mo_filename.str().c_str(), "wb");
    mpz_out_raw(mo_output, Utilities::StringToNumber(serial_str).get_mpz_t());
    fclose(mo_output);
    
    std::stringstream filename1;
    filename1 << "x.bin";
    FILE* xFile = fopen(filename1.str().c_str(), "rb");
    mpz_class xMpz;
    mpz_inp_raw(xMpz.get_mpz_t(), xFile );
    fclose(xFile);
        
    MoneyOrder new_mo;
    new_mo.Deserialize(Utilities::NumberToString(xMpz));

    BOOST_CHECK_EQUAL(new_mo.m_amount, old_mo.m_amount);
    BOOST_CHECK_EQUAL(new_mo.m_uniqueness, old_mo.m_uniqueness);
    BOOST_REQUIRE_EQUAL(new_mo.m_identity_strings.size(), 200U);
    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[0].first.first, "leftHash_0");
    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[0].first.second, "leftR1_0");
    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[0].second.first, "rightHash_0");
    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[0].second.second, "rightR1_0");

    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[1].first.first, "leftHash_1");
    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[1].first.second, "leftR1_1");
    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[1].second.first, "rightHash_1");
    BOOST_CHECK_EQUAL(new_mo.m_identity_strings[1].second.second, "rightR1_1");
}


BOOST_AUTO_TEST_CASE(money_order_info_test_1)
{
    std::vector<MoneyOrderInfo> info_vec;
    MoneyOrderInfo info;
    info.m_blinding_factor = "2394262445592642632306707632461079788577338370913227907193177833182985877544234442800057994459481910350241788357623265528469560138808620624954012423617897";

    mpz_class left;
    mpz_class right;
    std::tie(left, right) = SecretSplitting::SplitSecret("this is a test");

    CommitData leftCommitData = GenCommitData(left);
    std::string leftHash = Hash( leftCommitData );

    CommitData rightCommitData = GenCommitData(right);
    std::string rightHash = Hash( rightCommitData );

    info.m_commit_data.push_back(std::pair<CommitData, CommitData>(leftCommitData, rightCommitData));
    info.m_commit_data.push_back(std::pair<CommitData, CommitData>(leftCommitData, rightCommitData));
    info_vec.push_back(info);

    std::string ser = info_vec[0].Serialize();

    MoneyOrderInfo new_info;
    new_info.Deserialize(ser);

    BOOST_CHECK_EQUAL(info.m_blinding_factor, new_info.m_blinding_factor);
    BOOST_CHECK(info.m_commit_data[0].first.b == new_info.m_commit_data[0].first.b);
    BOOST_CHECK(info.m_commit_data[0].second.b == new_info.m_commit_data[0].second.b);
    BOOST_CHECK(info.m_commit_data[1].first.b == new_info.m_commit_data[1].first.b);
    BOOST_CHECK(info.m_commit_data[1].second.b == new_info.m_commit_data[1].second.b);
}

