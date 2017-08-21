// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2013-2017 The Feathercoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;

#include "chainparamsseeds.h"

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
	protected:
    Consensus::Params hardforkThree;
    Consensus::Params hardforkFour;
    	
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 2100000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit =    uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powNeoLimit = uint256S("0000003fffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // 3.5 days
        consensus.nPowTargetSpacing = 2.5 * 60; // 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = false;
        
        hardforkThree = consensus;
        hardforkThree.nHeightEffective = 204639;
        hardforkThree.nPowTargetTimespan = 60; // 1 minute timespan
        hardforkThree.nPowTargetTimespan = 60; // 1 minute block
        
        hardforkFour = hardforkThree;
        hardforkFour.nHeightEffective = 432000;
        hardforkFour.powLimit = uint256S("0000003fffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        
        // Assemble the binary search tree of consensus parameters
        pConsensusRoot = &hardforkThree;
        hardforkThree.pLeft = &consensus;
        hardforkThree.pRight = &hardforkFour;
        
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xfb;
        pchMessageStartNew[0] = 0xfe;
        pchMessageStart[1] = 0xc0;
        pchMessageStartNew[1] = 0x46;
        pchMessageStart[2] = 0xb6;
        pchMessageStartNew[2] = 0x54;
        pchMessageStart[3] = 0xdb;
        pchMessageStartNew[3] = 0x43;

        vAlertPubKey = ParseHex("043c19a29fe8f763369aea68107e82854af7b072fc7d2d2adb87d2a3b40b51ab0d0e77805096e255a87388b175fd4a49d93d9b6c878004975e41222a3b85086eef");
        nDefaultPort = 9336;
        nMinerThreads = 0;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 100000;

        /**
         * Build the genesis block. Note that the output of its generation
         * transaction cannot be spent since it did not originally exist in the
         * database.
         *
         * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
         *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
         *   vMerkleTree: 4a5e1e
         */
        const char* pszTimestamp = "NY Times 05/Oct/2011 Steve Jobs, Apple’s Visionary, Dies at 56";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 50 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1317972665;
        genesis.nBits    = 0x1e0ffff0;
        genesis.nNonce   = 2084524493;

        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x12a765e31ffd4059bada1e25190f6e98c99d9714d334efa41a195a7e7e04bfe2"));
        assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));


        vSeeds.push_back(CDNSSeedData("feathercoin.com", "dnsseed.feathercoin.com"));
        vSeeds.push_back(CDNSSeedData("ftc-c.com", "dnsseed.ftc-c.com"));
        

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,14);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,96);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,142);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xBC)(0x26).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xDA)(0xEE).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            ( 11111, uint256S("0x150efec427f0c53e0732b4a7ae88bd12c56e90c5577e5b0a86d19f8341ef733d"))
            ( 22267, uint256S("0x23dc7d871fc2a9b994112e978019f6370bab0b8979f557afe77a7ab620224b70"))
            ( 33001, uint256S("0x0a9d974a327961025a6b1dcbb70f980da2e61cf2c7079df6a73ac834fc3f8bc4"))
            ( 34000, uint256S("0x082f5d9023af3f068733ab68cf81b741f58e3c75ae28d2a9bd07f30b74c38356"))
            ( 87948, uint256S("0xcc090355706db5bff83db9a0623b5cc4febd317e5bd07c9bb5a7ee22a7891810"))
            (105000, uint256S("0xf88127af8f17ddd0dcfe1c052dba95033cde83269ced952700e58d1dd7209f30"))
            (134444, uint256S("0x7fea1270611335aa5cce27c1cdfe1c7d27dbe3528d606013f2bd271d15b35943"))
            (168000, uint256S("0x966b27809cf680558b4ec1265eeb18b16cad0ac89c95c213059457e970cd11a8"))
            (193000, uint256S("0x4238259cac9a326a051d761994147e83de9e88a9256caedb16e25cacfda79170"))
            (210000, uint256S("0x71f50b6380916a35b11da6ea1f5978ce52af13c06eb514795d3e3aeced7a2c1a"))
            (216116, uint256S("0x0d44239447a8fc21c18087237ffa359fdffe9f0714c997fcc3bac4bf49d06ceb"))
            (225430, uint256S("0x75565a0b2673a06bfcd70d80c490d0d90fd31f8768e9e5fbffb85047673720c2"))
            (250000, uint256S("0x3bbf886d14cc2f00c06ab22a46329a3fe6837b6ac5f504a85339be91e9b4fbe9"))
            (432001, uint256S("0x37ae6928845fb6aa4e8f62135557e056621b72b7c0c773c2eeecc7d19a78d0f4"))
            (1114311,uint256S("0x93515f222f16a9ff3db6594e5ee7c12924cff9ba05b01dbe551d0a9e65dd141f"))
            (1142737,uint256S("0x93515f222f16a9ff3db6594e5ee7c12924cff9ba05b01dbe551d0a9e65dd141f"))
            (1776411,uint256S("0x4f6de194bd2f4580e2ac9337289c7cca348d3f35c9079af2760b288315b82feb")),
            1498636263, // * UNIX timestamp of last checkpoint block 1776411
            3660284,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            1500.0        // * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nMajorityEnforceBlockUpgrade = 510;
        consensus.nMajorityRejectBlockOutdated = 750;
        consensus.nMajorityWindow = 1000;
        consensus.fPowAllowMinDifficultyBlocks = true;
        
        pchMessageStart[0] = 0xda;
        pchMessageStartNew[0] = 0xaa;
        pchMessageStart[1] = 0xaf;
        pchMessageStartNew[1] = 0xbb;
        pchMessageStart[2] = 0xa5;
        pchMessageStartNew[2] = 0xcc;
        pchMessageStart[3] = 0xba;
        pchMessageStartNew[3] = 0xdd;
        
	vAlertPubKey = ParseHex("04e7b36458cb1db28567a99391109bc55a0c55623836d93d8794db6549dcc590012d1f5e23c786b752650dadce34fe5504dd7332450392eeb8292e62b211920c78");
        nDefaultPort = 19336;
        nMinerThreads = 0;
        nMaxTipAge = 0x7fffffff;
        nPruneAfterHeight = 1000;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime  = 1396255061;
        genesis.nNonce = 3250989159;
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x8e8b634d2f2800398261b7adcfbb6ace490e1746e62123ec2bf8010f9fc98b17"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("feathercoin.com", "dnsseed.feathercoin.com"));
        vSeeds.push_back(CDNSSeedData("dnsseed-testnet.feathercoin.com", "testnet.ftc-c.com"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,65);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,193);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            ( 33, uint256S("5436731f1fc3efe4943fbdd5f680c5c861dc7de8b4f5de3c8f97423d8ab39798")),
            1486723363,
            1488,
            300
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nMinerThreads = 1;
        nMaxTipAge = 24 * 60 * 60;
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 2;
        consensus.hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 29336;
        //printf("genesis.hash: %s\n",consensus.hashGenesisBlock.ToString().c_str());  //run coind.exe
        assert(consensus.hashGenesisBlock == uint256S("0x9372df8b4c0144d2238b73d65ce81b5eb37ec416c23fc29307b89de4b0493cf8"));
        nPruneAfterHeight = 1000;

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206")),
            0,
            0,
            0
        };
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
