// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//
// Unit tests for block-chain checkpoints
//

#include "checkpoints.h"

#include "uint256.h"

#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p1 = uint256("0xfdbe99b90c90bae7505796461471d89ae8388ab953997aa06a355bbda8d915cb");
    uint256 p41300 = uint256("0x8c4e02f6c0d20e856fd7e952a147fee30ce145ca6932a284f354924362d2b408");
    BOOST_CHECK(Checkpoints::CheckBlock(1, p1));
    BOOST_CHECK(Checkpoints::CheckBlock(41300, p41300));

    
    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(1, p41300));
    BOOST_CHECK(!Checkpoints::CheckBlock(41300, p1));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(1+1, p41300));
    BOOST_CHECK(Checkpoints::CheckBlock(41300+1, p1));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 40000);
}    

BOOST_AUTO_TEST_SUITE_END()
