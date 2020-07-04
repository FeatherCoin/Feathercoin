// Copyright (c) 2013-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>

#include <clientversion.h>
#include <key.h>
#include <key_io.h>
#include <streams.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <test/setup_common.h>

#include <string>
#include <vector>

struct TestDerivation {
    std::string pub;
    std::string prv;
    unsigned int nChild;
};

struct TestVector {
    std::string strHexMaster;
    std::vector<TestDerivation> vDerive;

    explicit TestVector(std::string strHexMasterIn) : strHexMaster(strHexMasterIn) {}

    TestVector& operator()(std::string pub, std::string prv, unsigned int nChild) {
        vDerive.push_back(TestDerivation());
        TestDerivation &der = vDerive.back();
        der.pub = pub;
        der.prv = prv;
        der.nChild = nChild;
        return *this;
    }
};

TestVector test1 =
  TestVector("000102030405060708090a0b0c0d0e0f")
    ("xq1voqNse7SRu45ADa3URoneqtJfs6ZvjD67H2Bs8kWZNtZEVb7cVBgu5qm9d6WAVKM8YM1uxf85YNraw87cREF48KA2LxUmL4J4M1ViMbrVU34",
     "xqMPAnwzqh1qiKsaPSTrrcZHNkDxydXF1gszaFNP9iv9jYcWRpL1u9gEX3iJ4iJkekoey8Q7tVyxyGf2L5n1oAtsgAK72Hqf9Y9A2kRG3a5Gesq",
     0x80000000)
    ("xq1voseHdpjtzKC9NBBBdZfHivQnBv8H7Ha7xdcm6qtgM5kWrsQnuRDzCz7ZQN4sEdFfYZJUptfWH1vfuX5kNFySCCckYbH2FNCGrwdGfE7R6qV",
     "xqMPAqDQqQKJoazZY3ba4NRvFnL5JT5bPmN1FroH7pJGhjono6dCKPDKeC4hqyf64UBcgBHpFoJqMdVu3qojSmB4UEPd6AWoPVr3jm47UhPZ2Vx",
     1)
    ("xq1voupQqcHnP25DCdDdMpZAKfJSFJ7P7KJQtxHbh7fVHiYj6QuvdDeJAZ1npSXSeZy1WGvXcK7cPcR9VppZqb9KsrSgqo7sj1CYVgA3xBDRbSF",
     "xqMPAsPY3BsCCHsdNVe1ndKnrXDjMq4hPo6JCBU7i655eNc12e8L3BddbkxwG2r9fPxf8G3W4cuGSLTKBnKxa6y3FPt4bwUXTFcuQZNV1cMJNAr",
     0x80000002)
    ("xq1voxRgt97e5tvd4o1icQxXgfLaUSyuNrtQEVXGwSxeTavAWG73RyUj4zZiNTiH6SQKro2GDh6DmYz7x9xrYggrwJbXRh61oxGEDN5C3Rg7det",
     "xqMPAuzp5ih3uAj3EfS73DjADXFsaywDfLgHXihnxRNEpEySSVKSqwU4WCWrp54mddU9dj4tf5rBrkHDQz4iA7tENVKixZ35cBrr1q7heDGSzoY",
     2)
    ("xq1vozf5iaEbGNbc9GVSJgAcaAVtZGGe1TCbfyQaCQV7n2UmsMjQVkB6Ga2e3nhfWEZ4L79SMhaApHtEWaJrdz84eu4nLUeCe9A4E3z3C585Rvi",
     "xqMPAxECv9p15eQ2K8upjUwF72RBfoDxHvzUyCb6DNti8gY3oawouiARhmynVPUE6HsahXCbH15kmZjNtAyAMYRaZsumMo8BAGgZAoas9ks7Mch",
     1000000000)
    ("xq1vp2NrCFqqPVyofcGtRDjLssVtMVqwpx1dUDo8SgsFz6sGaV8o2ARkXgBhgcccMVdmooRys3VyMVaq6zhsjqcuWADuD24HScjh31bxS8W5kiZ",
     "xqMPAywyPqRFCmnDqUhGr2VyQjRBU2oG7RoWmSyeTfGrLkvYWiMCS8R5xt8r8FFS1CzfJvbr4N4hccGVtLyXtcSJgqZJhHtQHBSGT99DtryXkpW",
     0);

TestVector test2 =
  TestVector("fffcf9f6f3f0edeae7e4e1dedbd8d5d2cfccc9c6c3c0bdbab7b4b1aeaba8a5a29f9c999693908d8a8784817e7b7875726f6c696663605d5a5754514e4b484542")
    ("xq1voqNse7SRu45ADBYoXxrwJ62S7HiMQqLvxbybRhFPMGg6na8ofLTqWjvjyCb9CZi6SuFhsKnEwTMqeuqqYRADGE9yCtgEAYnifPApdWeiTxL",
     "xqMPAnwzqh1qiKsaP3yBxmdZpwwjDpffhK8pFqA7SfeyhvjNioMD5JTAwwstQn5oqGMkDB9YdnSRFNhDRcW7qbHaHXdsUhuuHt3tc13VQX8NYaD",
     0)
    ("xq1votecuq8PP4uEjT96fkEpMtRxUFTc7oqDWmixbWGqRgY4UYanEboufaVzYfZ2zeiXfDjsjYmzLYCop5GXRZrdbppFUwQu4tw6nsHV4gVry6c",
     "xqMPArDk7QhoCLheuKZV6Z1StkMFanQvQHd6ozuUcUgRnLbLQmoBeZoF6nT8zHMcULCacqPNXnr9gY7Ln88wPsndDkoGUtKxgNDdjQx3mnioRGL",
     0xFFFFFFFF)
    ("xq1vouogAS9uky5LuKMH6CETqE7WrzgWfHzubvoVWuBc5RJSvqpErVEDseHbA3o9kPKdSjfRPPdbdkdQUk8sCULkKyGt222xRdC7fFZ5uYLpLcx",
     "xqMPAsNoN1jKaEsm5BmfX116N62oyXdpwmnnu9z1XsbCS5Mis52eGTDZJrEjbdqbCaw4X4iGJp3njYcTDMsi8FPko4EpucZJSswtXCHaYG2iGdb",
     1)
    ("xq1voxceaT8qvcNXzw2R9rP2oB3kCXZ1mUvAvNJMMxqwPJetbfgQ9zkvmYfxzqzHYG1v2v7yE2iqGswEZYPt7NmhbJjncJEoeAdKb2E39uqTqKC",
     "xqMPAvBmn2iFjtAxAoSoaf9fL2y3K4WL3xi4DbUsNwFXjxiAXttoZxkGCkd7SS3DEJ8kq24y478B96HfY4esB8v8oY483P67HyvGzziK1wegNzZ",
     0xFFFFFFFE)
    ("xq1voyngVR5CY1LFHtG1tLCaAgjw7G2NpTYfx52EtgYME4v9421LFfgwJ534dW7njdYMzMmc2XnYKVG6DBZGoFtDJfSEtCZJCTijTFJcgsg3NM8",
     "xqMPAwMogzecMH8fTkgQK8yChYfEDnyh6wLZFJCkuewwaiyQzFDjfdgGjGzD58mbriX3NbMfMvuAXTsFMBkEh5pdVNjHsKdRSBwuLaiq5drcP7f",
     2)
    ("xq1vp19iSrHi4BeV39SucQLG9S7uWKZV4nTkcEe8JTtqrrmj8DLuRJchUdz4huC4sCu4WQyqJTHxDCH5hccmdMa7cvHpXNPP8q1U8YVoMZgETkj",
     "xqMPAxiqeRs7sTSuD1sJ3D6tgJ3CcrWoMGFduTpeKSJSDWq14SZJqGc2uqwD9YSc1jc7QuDPegXyyZhwPjdkRQBrYMU1gB8vVc5d2x4PAJ7yCcs",
     0);

TestVector test3 =
  TestVector("4b381541583be4423346c643850da4b320e46a87ae3d2a4e6da11eba819cd4acba45d239319ac14f863b8d5ab5a0d0c64d2e8a1e7d1457df2e5a3c51c73235be")
    ("xq1voqNse7SRu45ACEzy4MMjW5HTyw3TAhWCfwk9pzGwcxktkpN5KUhoU1AGTC882AHjcz1ZhKfFL8Bh6DUkP8CegZp7L8aSu7EZT6gX9ygCPSq",
     "xqMPAnwzqh1qiKsaN7RMVA8N2wCm6TzmTBJ5yAvfqxgXycpAh3aUjSh8uD7QtmoynHb5HspJCi8X9oRvnG7S52S15197zzUYVKPrtihr4TS8XTf",
      0x80000000)
    ("xq1vosk5P5GZaRSWPn6NHVhEZL38gHeqKYv75Rz6eDxnZGgG1qVXtyUfYTwykuxdkWtUpMWQUHQzB5PBigR6KN8qFp641EXLSxjMwH3RpNNJfyK",
     "xqMPAqKCaeqyPhEvZeWkiJTs6BxRnpc9c2hzNfAcfCNNuvjXx4hwJwTzyfu8CYAL8Euzk2TiDevPoY4PRokaeF1kpmLkS8PbEUfg1jfW1sbreRs",
      0);

static void RunTest(const TestVector &test) {
    std::vector<unsigned char> seed = ParseHex(test.strHexMaster);
    CExtKey key;
    CExtPubKey pubkey;
    key.SetSeed(seed.data(), seed.size());
    pubkey = key.Neuter();
    for (const TestDerivation &derive : test.vDerive) {
        unsigned char data[74];
        key.Encode(data);
        pubkey.Encode(data);

        // Test private key
        BOOST_CHECK(EncodeExtKey(key) == derive.prv);
        BOOST_CHECK(DecodeExtKey(derive.prv) == key); //ensure a base58 decoded key also matches

        // Test public key
        BOOST_CHECK(EncodeExtPubKey(pubkey) == derive.pub);
        BOOST_CHECK(DecodeExtPubKey(derive.pub) == pubkey); //ensure a base58 decoded pubkey also matches

        // Derive new keys
        CExtKey keyNew;
        BOOST_CHECK(key.Derive(keyNew, derive.nChild));
        CExtPubKey pubkeyNew = keyNew.Neuter();
        if (!(derive.nChild & 0x80000000)) {
            // Compare with public derivation
            CExtPubKey pubkeyNew2;
            BOOST_CHECK(pubkey.Derive(pubkeyNew2, derive.nChild));
            BOOST_CHECK(pubkeyNew == pubkeyNew2);
        }
        key = keyNew;
        pubkey = pubkeyNew;

        CDataStream ssPub(SER_DISK, CLIENT_VERSION);
        ssPub << pubkeyNew;
        BOOST_CHECK(ssPub.size() == 75);

        CDataStream ssPriv(SER_DISK, CLIENT_VERSION);
        ssPriv << keyNew;
        BOOST_CHECK(ssPriv.size() == 75);

        CExtPubKey pubCheck;
        CExtKey privCheck;
        ssPub >> pubCheck;
        ssPriv >> privCheck;

        BOOST_CHECK(pubCheck == pubkeyNew);
        BOOST_CHECK(privCheck == keyNew);
    }
}

BOOST_FIXTURE_TEST_SUITE(bip32_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(bip32_test1) {
    RunTest(test1);
}

BOOST_AUTO_TEST_CASE(bip32_test2) {
    RunTest(test2);
}

BOOST_AUTO_TEST_CASE(bip32_test3) {
    RunTest(test3);
}

BOOST_AUTO_TEST_SUITE_END()
