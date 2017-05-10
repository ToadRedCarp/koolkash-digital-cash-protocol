#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/results_collector.hpp>
#include "BitCommitment.h"
#include <iostream>
#include "Utilities.h"

#include <omp.h>

BOOST_AUTO_TEST_CASE(BitCommitment_test_1)
{
    mpz_class b = Utilities::StringToNumber("abc");

    double start = omp_get_wtime();
    CommitData d = GenCommitData(b);
    double end = omp_get_wtime();

    std::cout << "Bit Commitment Test 1 Timing " << end - start << "s" << std::endl;

    std::string commitHash = Hash(d);
    std::string commitR1   = d.r1;

    // Life moves on, d is suddenly revealed, and then...
    mpz_class commitHashMpz = Utilities::StringToNumber(commitHash);
    mpz_class r1Mpz = Utilities::StringToNumber(commitR1);

    BOOST_CHECK( Verify(d,commitHash,commitR1));
}

BOOST_AUTO_TEST_CASE(BitCommitment_test_2)
{
    mpz_class b = Utilities::StringToNumber("abcdefghijklmnopqrstuvwxyz");

    double start = omp_get_wtime();
    CommitData d = GenCommitData(b);
    double end = omp_get_wtime();

    std::cout << "Bit Commitment Test 2 Timing " << end - start << "s" << std::endl;

    std::string commitHash = Hash(d);
    std::string commitR1   = d.r1;

    std::string serialStr = d.Serialize();
    CommitData newD;
    newD.Deserialize(serialStr);

    // Life moves on, d is suddenly revealed, and then...
    BOOST_CHECK( Verify(newD,commitHash,commitR1));
}

