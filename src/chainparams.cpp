// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "assert.h"
#include "core.h"
#include "protocol.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

//
// Main network
//

unsigned int pnSeed[] = 
{
    0xc49c0ed8, 0x1df9d243, 0xd9b75a40, 0xdc525e46, 0x72c321b1, 0x4eedeeb2, 0x18271787, 0xce725232,
    0x892aafc0, 0x979f6751, 0x2210f618, 0xfb529ed8, 0x66a74b3e, 0xef5d132e, 0x3b7a116c, 0x2fe45f55,
    0x1df9d243, 0xe41f7c70, 0x8f4cd262, 0xb5c29d62, 0x80f1f3a2, 0x47dc614d, 0x6f458b4e, 0x908caa56,
    0x553ef762, 0x5aec0852, 0x629a4f54, 0x6a10e13c, 0x4f41c547, 0x4476fd59, 0xcaedbc5a, 0x9f806dc1,
    0x1df9d243, 0xe41f7c70, 0x8f4cd262, 0xb5c29d62, 0x80f1f3a2, 0x47dc614d, 0x6f458b4e, 0x908caa56,
    0x553ef762, 0x5aec0852, 0x629a4f54, 0x6a10e13c, 0x4f41c547, 0x4476fd59, 0xcaedbc5a, 0x9f806dc1
};

class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xfb;
        pchMessageStart[1] = 0xc0;
        pchMessageStart[2] = 0xb6;
        pchMessageStart[3] = 0xdb;

        pchMessageStartNew[0] = 0xfe;
        pchMessageStartNew[1] = 0x46;
        pchMessageStartNew[2] = 0x54;
        pchMessageStartNew[3] = 0x43;
        //how to do
        //vAlertPubKey = ParseHex("04d4da7a5dae4db797d9b0644d57a5cd50e05a70f36091cd62e2fc41c98ded06340be5a43a35e185690cd9cde5d72da8f6d065b499b06f51dcfba14aad859f443a");
        vAlertPubKey = ParseHex("043c19a29fe8f763369aea68107e82854af7b072fc7d2d2adb87d2a3b40b51ab0d0e77805096e255a87388b175fd4a49d93d9b6c878004975e41222a3b85086eef");
        nDefaultPort = 9336;
        nRPCPort = 9337;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20);
        nSubsidyHalvingInterval = 2100000; //half every 2100000 blocks

        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.
        //
        // Genesis Block:
        // CBlock(hash=12a765e31ffd4059bada, PoW=0000050c34a64b415b6b, ver=1, hashPrevBlock=00000000000000000000, hashMerkleRoot=97ddfbbae6, nTime=1317972665, nBits=1e0ffff0, nNonce=2084524493, vtx=1)
        //   CTransaction(hash=97ddfbbae6, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        //     CTxIn(COutPoint(0000000000, -1), coinbase 04ffff001d0104404e592054696d65732030352f4f63742f32303131205374657665204a6f62732c204170706c65e280997320566973696f6e6172792c2044696573206174203536)
        //     CTxOut(nValue=50.00000000, scriptPubKey=040184710fa689ad5023690c80f3a4)
        //   vMerkleTree: 97ddfbbae6
        const char* pszTimestamp = "NY Times 05/Oct/2011 Steve Jobs, Apple’s Visionary, Dies at 56";
        CTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 50 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1317972665;
        genesis.nBits    = 0x1e0ffff0;
        genesis.nNonce   = 2084524493;
        
        hashGenesisBlock = genesis.GetHash();
        assert(genesis.hashMerkleRoot == uint256("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));
        assert(hashGenesisBlock == uint256("0x12a765e31ffd4059bada1e25190f6e98c99d9714d334efa41a195a7e7e04bfe2"));

        vSeeds.push_back(CDNSSeedData("feathercoin.com", "dnsseed.feathercoin.com"));
        vSeeds.push_back(CDNSSeedData("explorer2.feathercoin.com", "explorer2.feathercoin.com"));
        vSeeds.push_back(CDNSSeedData("alltheco.in", "dnsseed.alltheco.in"));
        vSeeds.push_back(CDNSSeedData("ftc-c.com", "block.ftc-c.com"));


        //how to do feathercoin ?
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,14); // FeatherCoin addresses start with F
        base58Prefixes[SCRIPT_ADDRESS_OLD] = std::vector<unsigned char>(1,5);
      //  base58Prefixes[SCRIPT_ADDRESS] = list_of(96);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,142);// 14+128
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xBC)(0x26).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xDA)(0xEE).convert_to_container<std::vector<unsigned char> >();
        //base58Prefixes[EXT_SCRIPT_ADDRESS] = list_of(96);
        
        // Convert the pnSeeds array into usable address objects.
        for (unsigned int i = 0; i < ARRAYLEN(pnSeed); i++)
        {
            // It'll only connect to one or two seed nodes because once it connects,
            // it'll get a pile of addresses with newer timestamps.
            // Seed nodes are given a random 'last seen time' of between one and two
            // weeks ago.
            const int64_t nOneWeek = 7*24*60*60;
            struct in_addr ip;
            memcpy(&ip, &pnSeed[i], sizeof(ip));
            CAddress addr(CService(ip, GetDefaultPort()));
            addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
            vFixedSeeds.push_back(addr);
        }
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet (v3)
//
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xda;
        pchMessageStart[1] = 0xaf;
        pchMessageStart[2] = 0xa5;
        pchMessageStart[3] = 0xba;
	
	pchMessageStartNew[0] = 0xaa;
        pchMessageStartNew[1] = 0xbb;
        pchMessageStartNew[2] = 0xcc;
        pchMessageStartNew[3] = 0xdd;
        //how to do
        vAlertPubKey = ParseHex("04e7b36458cb1db28567a99391109bc55a0c55623836d93d8794db6549dcc590012d1f5e23c786b752650dadce34fe5504dd7332450392eeb8292e62b211920c78");
        nDefaultPort = 19336;
        nRPCPort = 19337;
        strDataDir = "testnet3";

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime    = 1396255061;
        genesis.nNonce   = 3250989159;
        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x8e8b634d2f2800398261b7adcfbb6ace490e1746e62123ec2bf8010f9fc98b17"));

        vFixedSeeds.clear();
        vSeeds.clear();
        //how to do feathercoin
        //vSeeds.push_back(CDNSSeedData("bitcoin.petertodd.org", "testnet-seed.bitcoin.petertodd.org"));
        vSeeds.push_back(CDNSSeedData("explorer2.feathercoin.com","feathercoin.com"));
        vSeeds.push_back(CDNSSeedData("testnet-dnsseed.feathercoin.com","feathercoin.com"));

        // Boost sucks, and should not be used. Workaround for Boost not being compatible with C++11;
        
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,65);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,193);//65+128
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }
    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nSubsidyHalvingInterval = 150;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 2;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18444;
        strDataDir = "regtest";
        assert(hashGenesisBlock == uint256("0x8e8b634d2f2800398261b7adcfbb6ace490e1746e62123ec2bf8010f9fc98b17"));

        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
    }

    virtual bool RequireRPCPassword() const { return false; }
    virtual Network NetworkID() const { return CChainParams::REGTEST; }
};
//static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CChainParams::REGTEST:
            //pCurrentParams = &regTestParams;
            pCurrentParams = &testNetParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest) {
        return false;
    }

    if (fRegTest) {
        SelectParams(CChainParams::REGTEST);
    } else if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
