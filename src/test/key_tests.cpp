// Copyright (c) 2012-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <key.h>

#include <key_io.h>
#include <uint256.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <test/setup_common.h>

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

static const std::string strSecret1 = "5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj";
static const std::string strSecret2 = "5mSnMx3PpVJRoutEHL3NZyWXXUNGCaYc66YuqtsnuhJbhzhgDWv";
static const std::string strSecret1C = "N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq";
static const std::string strSecret2C = "N3AoDTwZibuvW4R7d53UCYRKjVnr674VsVVrEoyBdatgBcsFTSd7";
static const std::string addr1 = "6vJPYFvgXL3vuWXXJ6bZN5Zyd7gndnMSyv";
static const std::string addr2 = "6ogtBmTsLZGWvfX9xP7E5JfZiqWcnSkF8Z";
static const std::string addr1C = "6snJxHGbqTpRvrgKxuoH41muC72hAx3rpM";
static const std::string addr2C = "6fKbFKqRUfuM8xGdp9qyXAfPCANPU75QRA";

static const std::string strAddressBad = "6snJxHGbqTpRvrgKxuoH41muC72hAx3rp1";


BOOST_FIXTURE_TEST_SUITE(key_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(key_test1)
{
    CKey key1  = DecodeSecret(strSecret1);
    BOOST_CHECK(key1.IsValid() && !key1.IsCompressed());
    CKey key2  = DecodeSecret(strSecret2);
    BOOST_CHECK(key2.IsValid() && !key2.IsCompressed());
    CKey key1C = DecodeSecret(strSecret1C);
    BOOST_CHECK(key1C.IsValid() && key1C.IsCompressed());
    CKey key2C = DecodeSecret(strSecret2C);
    BOOST_CHECK(key2C.IsValid() && key2C.IsCompressed());
    CKey bad_key = DecodeSecret(strAddressBad);
    BOOST_CHECK(!bad_key.IsValid());

    CPubKey pubkey1  = key1. GetPubKey();
    CPubKey pubkey2  = key2. GetPubKey();
    CPubKey pubkey1C = key1C.GetPubKey();
    CPubKey pubkey2C = key2C.GetPubKey();

    BOOST_CHECK(key1.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key1C.VerifyPubKey(pubkey1));
    BOOST_CHECK(key1C.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key1C.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key1C.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key2.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key2.VerifyPubKey(pubkey1C));
    BOOST_CHECK(key2.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key2.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key2C.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key2C.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key2C.VerifyPubKey(pubkey2));
    BOOST_CHECK(key2C.VerifyPubKey(pubkey2C));

    BOOST_CHECK(DecodeDestination(addr1)  == CTxDestination(PKHash(pubkey1)));
    BOOST_CHECK(DecodeDestination(addr2)  == CTxDestination(PKHash(pubkey2)));
    BOOST_CHECK(DecodeDestination(addr1C) == CTxDestination(PKHash(pubkey1C)));
    BOOST_CHECK(DecodeDestination(addr2C) == CTxDestination(PKHash(pubkey2C)));

    for (int n=0; n<16; n++)
    {
        std::string strMsg = strprintf("Very secret message %i: 11", n);
        uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

        // normal signatures

        std::vector<unsigned char> sign1, sign2, sign1C, sign2C;

        BOOST_CHECK(key1.Sign (hashMsg, sign1));
        BOOST_CHECK(key2.Sign (hashMsg, sign2));
        BOOST_CHECK(key1C.Sign(hashMsg, sign1C));
        BOOST_CHECK(key2C.Sign(hashMsg, sign2C));

        BOOST_CHECK( pubkey1.Verify(hashMsg, sign1));
        BOOST_CHECK(!pubkey1.Verify(hashMsg, sign2));
        BOOST_CHECK( pubkey1.Verify(hashMsg, sign1C));
        BOOST_CHECK(!pubkey1.Verify(hashMsg, sign2C));

        BOOST_CHECK(!pubkey2.Verify(hashMsg, sign1));
        BOOST_CHECK( pubkey2.Verify(hashMsg, sign2));
        BOOST_CHECK(!pubkey2.Verify(hashMsg, sign1C));
        BOOST_CHECK( pubkey2.Verify(hashMsg, sign2C));

        BOOST_CHECK( pubkey1C.Verify(hashMsg, sign1));
        BOOST_CHECK(!pubkey1C.Verify(hashMsg, sign2));
        BOOST_CHECK( pubkey1C.Verify(hashMsg, sign1C));
        BOOST_CHECK(!pubkey1C.Verify(hashMsg, sign2C));

        BOOST_CHECK(!pubkey2C.Verify(hashMsg, sign1));
        BOOST_CHECK( pubkey2C.Verify(hashMsg, sign2));
        BOOST_CHECK(!pubkey2C.Verify(hashMsg, sign1C));
        BOOST_CHECK( pubkey2C.Verify(hashMsg, sign2C));

        // compact signatures (with key recovery)

        std::vector<unsigned char> csign1, csign2, csign1C, csign2C;

        BOOST_CHECK(key1.SignCompact (hashMsg, csign1));
        BOOST_CHECK(key2.SignCompact (hashMsg, csign2));
        BOOST_CHECK(key1C.SignCompact(hashMsg, csign1C));
        BOOST_CHECK(key2C.SignCompact(hashMsg, csign2C));

        CPubKey rkey1, rkey2, rkey1C, rkey2C;

        BOOST_CHECK(rkey1.RecoverCompact (hashMsg, csign1));
        BOOST_CHECK(rkey2.RecoverCompact (hashMsg, csign2));
        BOOST_CHECK(rkey1C.RecoverCompact(hashMsg, csign1C));
        BOOST_CHECK(rkey2C.RecoverCompact(hashMsg, csign2C));

        BOOST_CHECK(rkey1  == pubkey1);
        BOOST_CHECK(rkey2  == pubkey2);
        BOOST_CHECK(rkey1C == pubkey1C);
        BOOST_CHECK(rkey2C == pubkey2C);
    }

    // test deterministic signing

    std::vector<unsigned char> detsig, detsigc;
    std::string strMsg = "Very deterministic message";
    uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());
    BOOST_CHECK(key1.Sign(hashMsg, detsig));
    BOOST_CHECK(key1C.Sign(hashMsg, detsigc));
    BOOST_CHECK(detsig == detsigc);
    BOOST_CHECK(detsig == ParseHex("304402204aa0360928f9fda1169c0f1e91514c937da5816ccc21215a6e9bc4437cc2fae302205b242ad273ec80d338ab7bf8922c5b8702b796876d9a01384b4f2e3f6caaf924"));
    BOOST_CHECK(key2.Sign(hashMsg, detsig));
    BOOST_CHECK(key2C.Sign(hashMsg, detsigc));
    BOOST_CHECK(detsig == detsigc);
    BOOST_CHECK(detsig == ParseHex("304402202af410b032eb838ddc561b90fa098e0d9a0ec7ef64d87ca5ab042d8b2a273a46022074fbf0413ad7c607d3c68e87a3d232ee9660b06a360b1a164d4853c29a3e4e39"));
    BOOST_CHECK(key1.SignCompact(hashMsg, detsig));
    BOOST_CHECK(key1C.SignCompact(hashMsg, detsigc));
    BOOST_CHECK(detsig == ParseHex("1b4aa0360928f9fda1169c0f1e91514c937da5816ccc21215a6e9bc4437cc2fae35b242ad273ec80d338ab7bf8922c5b8702b796876d9a01384b4f2e3f6caaf924"));
    BOOST_CHECK(detsigc == ParseHex("1f4aa0360928f9fda1169c0f1e91514c937da5816ccc21215a6e9bc4437cc2fae35b242ad273ec80d338ab7bf8922c5b8702b796876d9a01384b4f2e3f6caaf924"));
    BOOST_CHECK(key2.SignCompact(hashMsg, detsig));
    BOOST_CHECK(key2C.SignCompact(hashMsg, detsigc));
    BOOST_CHECK(detsig == ParseHex("1c2af410b032eb838ddc561b90fa098e0d9a0ec7ef64d87ca5ab042d8b2a273a4674fbf0413ad7c607d3c68e87a3d232ee9660b06a360b1a164d4853c29a3e4e39"));
    BOOST_CHECK(detsigc == ParseHex("202af410b032eb838ddc561b90fa098e0d9a0ec7ef64d87ca5ab042d8b2a273a4674fbf0413ad7c607d3c68e87a3d232ee9660b06a360b1a164d4853c29a3e4e39"));
}

BOOST_AUTO_TEST_CASE(key_signature_tests)
{
    // When entropy is specified, we should see at least one high R signature within 20 signatures
    CKey key = DecodeSecret(strSecret1);
    std::string msg = "A message to be signed";
    uint256 msg_hash = Hash(msg.begin(), msg.end());
    std::vector<unsigned char> sig;
    bool found = false;

    for (int i = 1; i <=20; ++i) {
        sig.clear();
        BOOST_CHECK(key.Sign(msg_hash, sig, false, i));
        found = sig[3] == 0x21 && sig[4] == 0x00;
        if (found) {
            break;
        }
    }
    BOOST_CHECK(found);

    // When entropy is not specified, we should always see low R signatures that are less than 70 bytes in 256 tries
    // We should see at least one signature that is less than 70 bytes.
    found = true;
    bool found_small = false;
    for (int i = 0; i < 256; ++i) {
        sig.clear();
        std::string msg = "A message to be signed" + std::to_string(i);
        msg_hash = Hash(msg.begin(), msg.end());
        BOOST_CHECK(key.Sign(msg_hash, sig));
        found = sig[3] == 0x20;
        BOOST_CHECK(sig.size() <= 70);
        found_small |= sig.size() < 70;
    }
    BOOST_CHECK(found);
    BOOST_CHECK(found_small);
}

BOOST_AUTO_TEST_CASE(key_key_negation)
{
    // create a dummy hash for signature comparison
    unsigned char rnd[8];
    std::string str = "Bitcoin key verification\n";
    GetRandBytes(rnd, sizeof(rnd));
    uint256 hash;
    CHash256().Write((unsigned char*)str.data(), str.size()).Write(rnd, sizeof(rnd)).Finalize(hash.begin());

    // import the static test key
    CKey key = DecodeSecret(strSecret1C);

    // create a signature
    std::vector<unsigned char> vch_sig;
    std::vector<unsigned char> vch_sig_cmp;
    key.Sign(hash, vch_sig);

    // negate the key twice
    BOOST_CHECK(key.GetPubKey().data()[0] == 0x03);
    key.Negate();
    // after the first negation, the signature must be different
    key.Sign(hash, vch_sig_cmp);
    BOOST_CHECK(vch_sig_cmp != vch_sig);
    BOOST_CHECK(key.GetPubKey().data()[0] == 0x02);
    key.Negate();
    // after the second negation, we should have the original key and thus the
    // same signature
    key.Sign(hash, vch_sig_cmp);
    BOOST_CHECK(vch_sig_cmp == vch_sig);
    BOOST_CHECK(key.GetPubKey().data()[0] == 0x03);
}

BOOST_AUTO_TEST_SUITE_END()
