// Copyright (c) 2012-2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "key.h"

#include "base58.h"
#include "script.h"
#include "uint256.h"
#include "util.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

using namespace std;

static const string strSecret1     ("5mDrMmqC7Rg32vN5REY55krG8L5sYtvjouhQFT2fJSWf6YNoAQF");
static const string strSecret2     ("5nEw8kna8rJtg3geRwVBZc1eErJDzdHSabLT1F4yrVUdmYhPHC1");
static const string strSecret1C    ("N2Birgsi3MuHyUY2Vxk91zYRmMEW7nCBx3bKcmtL3ybu95ZRtUa5");
static const string strSecret2C    ("N6gVwEG9Fedy5LqzD5VPcuXqN8b2MM9ZuGYqn56ZwSgyHPTHUrLB");
static const CBitcoinAddress addr1 ("6mBjfXRA89XeSjTc1rzDQ1cWmggBvJ59NB");
static const CBitcoinAddress addr2 ("6jZqmkFVLY9LBgawnLqXUN5uj1NEHT3vc3");
static const CBitcoinAddress addr1C("71N4eaVLuWaqkQmBh64Pj9FtrHTRyauYwy");
static const CBitcoinAddress addr2C("6jBJB4kS8tWVCV85e5EfeV8UEB6rAEXRfX");


static const string strAddressBad("6j67A8CE6DMgKMY4q3fgjC4UQoNzrNUdXX");


#ifdef KEY_TESTS_DUMPINFO
void dumpKeyInfo(uint256 privkey)
{
    CKey key;
    key.resize(32);
    memcpy(&secret[0], &privkey, 32);
    vector<unsigned char> sec;
    sec.resize(32);
    memcpy(&sec[0], &secret[0], 32);
    printf("  * secret (hex): %s\n", HexStr(sec).c_str());

    for (int nCompressed=0; nCompressed<2; nCompressed++)
    {
        bool fCompressed = nCompressed == 1;
        printf("  * %s:\n", fCompressed ? "compressed" : "uncompressed");
        CBitcoinSecret bsecret;
        bsecret.SetSecret(secret, fCompressed);
        printf("    * secret (base58): %s\n", bsecret.ToString().c_str());
        CKey key;
        key.SetSecret(secret, fCompressed);
        vector<unsigned char> vchPubKey = key.GetPubKey();
        printf("    * pubkey (hex): %s\n", HexStr(vchPubKey).c_str());
        printf("    * address (base58): %s\n", CBitcoinAddress(vchPubKey).ToString().c_str());
    }
}
#endif


BOOST_AUTO_TEST_SUITE(key_tests)

BOOST_AUTO_TEST_CASE(key_test1)
{
    CBitcoinSecret bsecret1, bsecret2, bsecret1C, bsecret2C, baddress1;
    BOOST_CHECK( bsecret1.SetString (strSecret1));
    BOOST_CHECK( bsecret2.SetString (strSecret2));
    BOOST_CHECK( bsecret1C.SetString(strSecret1C));
    BOOST_CHECK( bsecret2C.SetString(strSecret2C));
    BOOST_CHECK(!baddress1.SetString(strAddressBad));
    
    bool fCompressed;
    
    CSecret secret1 = bsecret1.GetSecret(fCompressed);
    CKey key1;
    key1.SetSecret(secret1);
    BOOST_CHECK(key1.IsCompressed() == false);
    
    CSecret secret2 = bsecret2.GetSecret(fCompressed);
    CKey key2;
    key2.SetSecret(secret2);
    BOOST_CHECK(key2.IsCompressed() == false);
    
    
    CSecret secret1C = bsecret1C.GetSecret(fCompressed);
    CKey key1C;
    key1C.SetSecret(secret1C);
    BOOST_CHECK(key1C.IsCompressed() == true);
    
    CSecret secret2C = bsecret2C.GetSecret(fCompressed);
    CKey key2C;
    key2C.SetSecret(secret2C);
    BOOST_CHECK(key2C.IsCompressed() == true);

    CPubKey pubkey1  = key1. GetPubKey();
    CPubKey pubkey2  = key2. GetPubKey();
    CPubKey pubkey1C = key1C.GetPubKey();
    CPubKey pubkey2C = key2C.GetPubKey();

    BOOST_CHECK(addr1.Get()  == CTxDestination(pubkey1.GetID()));
    BOOST_CHECK(addr2.Get()  == CTxDestination(pubkey2.GetID()));
    BOOST_CHECK(addr1C.Get() == CTxDestination(pubkey1C.GetID()));
    BOOST_CHECK(addr2C.Get() == CTxDestination(pubkey2C.GetID()));

    for (int n=0; n<16; n++)
    {
        string strMsg = strprintf("Very secret message %i: 11", n);
        uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

        // normal signatures

        vector<unsigned char> sign1, sign2, sign1C, sign2C;

        BOOST_CHECK(key1.Sign (hashMsg, sign1));
        BOOST_CHECK(key2.Sign (hashMsg, sign2));
        BOOST_CHECK(key1C.Sign(hashMsg, sign1C));
        BOOST_CHECK(key2C.Sign(hashMsg, sign2C));

        BOOST_CHECK( key1.Verify(hashMsg, sign1));
        BOOST_CHECK(!key1.Verify(hashMsg, sign2));
        BOOST_CHECK( key1.Verify(hashMsg, sign1C));
        BOOST_CHECK(!key1.Verify(hashMsg, sign2C));

        BOOST_CHECK(!key2.Verify(hashMsg, sign1));
        BOOST_CHECK( key2.Verify(hashMsg, sign2));
        BOOST_CHECK(!key2.Verify(hashMsg, sign1C));
        BOOST_CHECK( key2.Verify(hashMsg, sign2C));

        BOOST_CHECK( key1C.Verify(hashMsg, sign1));
        BOOST_CHECK(!key1C.Verify(hashMsg, sign2));
        BOOST_CHECK( key1C.Verify(hashMsg, sign1C));
        BOOST_CHECK(!key1C.Verify(hashMsg, sign2C));

        BOOST_CHECK(!key2C.Verify(hashMsg, sign1));
        BOOST_CHECK( key2C.Verify(hashMsg, sign2));
        BOOST_CHECK(!key2C.Verify(hashMsg, sign1C));
        BOOST_CHECK( key2C.Verify(hashMsg, sign2C));

        // compact signatures (with key recovery)

        vector<unsigned char> csign1, csign2, csign1C, csign2C;

        BOOST_CHECK(key1.SignCompact (hashMsg, csign1));
        BOOST_CHECK(key2.SignCompact (hashMsg, csign2));
        BOOST_CHECK(key1C.SignCompact(hashMsg, csign1C));
        BOOST_CHECK(key2C.SignCompact(hashMsg, csign2C));

        CKey rkey1, rkey2, rkey1C, rkey2C;

        BOOST_CHECK(rkey1.SetCompactSignature (hashMsg, csign1));
        BOOST_CHECK(rkey2.SetCompactSignature (hashMsg, csign2));
        BOOST_CHECK(rkey1C.SetCompactSignature(hashMsg, csign1C));
        BOOST_CHECK(rkey2C.SetCompactSignature(hashMsg, csign2C));

        BOOST_CHECK(rkey1.IsCompressed() == pubkey1.IsCompressed());
        BOOST_CHECK(rkey2.IsCompressed()  == pubkey2.IsCompressed());
        BOOST_CHECK(rkey1C.IsCompressed() == pubkey1C.IsCompressed());
        BOOST_CHECK(rkey2C.IsCompressed() == pubkey2C.IsCompressed());
    }
}

BOOST_AUTO_TEST_SUITE_END()
