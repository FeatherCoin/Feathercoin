// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "NY Times 05/Oct/2011 Steve Jobs, Apple’s Visionary, Dies at 56";
    const CScript genesisOutputScript = CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

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
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 2100000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 432000;
        consensus.BIP34Hash = uint256S("0x966e30dd04d09232f6f690a04664cd3258abe43eeda2f2291d93706aa494aa54");
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powNeoScryptLimit = uint256S("0000003fffff0000000000000000000000000000000000000000000000000000");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // 3.5 days
        consensus.nPowTargetSpacing = 2.5 * 60; // 2.5 mins
        consensus.checkpointPubKey = "04c67c0114bc7cb8bb84ee0f3319e1df3339d335a15bdb04605cf2655d19212848a66d4535f3c91e943061474b7cacfd4eaa10835d35a8d4e431c68a4c4f5450ba";
        consensus.vAlertPubKey = ParseHex("043c19a29fe8f763369aea68107e82854af7b072fc7d2d2adb87d2a3b40b51ab0d0e77805096e255a87388b175fd4a49d93d9b6c878004975e41222a3b85086eef");
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 15120; // 75% of 20160 blocks, two weeks
        consensus.nMinerConfirmationWindow = 20160;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1519862400; // March 1st, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1551398400; // March 1st, 2019

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1519862400; // March 1st, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1551398400; // March 1st, 2019

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000001000");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xfb;
        pchMessageStart[1] = 0xc0;
        pchMessageStart[2] = 0xb6;
        pchMessageStart[3] = 0xdb;
        pchMessageStartNew[0] = 0x41;
        pchMessageStartNew[1] = 0x15;
        pchMessageStartNew[2] = 0x1a;
        pchMessageStartNew[3] = 0x21;
        nDefaultPort = 9336;
        nPruneAfterHeight = 100000;

        consensus.nForkOne = 33000;
        consensus.nForkTwo = 87948;
        consensus.nForkThree = 204639;
        consensus.nForkFour = 432000;
        consensus.nTimeLimit = 2313000;
        consensus.nVersionBitSwitch = 2120000;
        consensus.nNeoScryptSwitch = 1413936000;
        consensus.nNeoScryptFork = 1414346265;

        genesis = CreateGenesisBlock(1317972665, 2084524493, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x12a765e31ffd4059bada1e25190f6e98c99d9714d334efa41a195a7e7e04bfe2"));
        assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));

        // Note that of those with the service bits flag, most only support a subset of possible options
        vSeeds.push_back(CDNSSeedData("feathercoin.com", "dnsseed.feathercoin.com", false));
        vSeeds.push_back(CDNSSeedData("feathercoin.com", "explorer2.feathercoin.com", false));
        vSeeds.push_back(CDNSSeedData("alltheco.in", "dnsseed.alltheco.in", false));
        vSeeds.push_back(CDNSSeedData("ftc-c.com", "block.ftc-c.com", false));
        vSeeds.push_back(CDNSSeedData("feathercoin.ch", "dnsseed-static.feathercoin.ch", false));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,14);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,142);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xBC)(0x26).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xDA)(0xEE).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (     1, uint256S("0xfdbe99b90c90bae7505796461471d89ae8388ab953997aa06a355bbda8d915cb"))
            (     22267, uint256S("0x23dc7d871fc2a9b994112e978019f6370bab0b8979f557afe77a7ab620224b70"))
            (     22847, uint256S("0x1450b80c150fee1e657ee8309819276342c021fab5e6a20ccf5407f5e2218d0f"))
            (     23453, uint256S("0x7c25d3f9671e1d9400c9a1be2ff68e68db561ab85ae6b7020062d8d26da81e4c"))
            (     28230, uint256S("0xd3feb71f92c63c682f994d7c615adc425226c669a83d46012b32fc8d518b08bb"))
            (     31846, uint256S("0xba7d5c0e6d46f6448253290ce037e13975c13ca9c375ae854b6b2f85044fc0f9"))
            (     33918, uint256S("0x023cf4acfd8bf0114090a7ce048e79ac28152de78bb41f1277742904494e6c49"))
            (     34000, uint256S("0x082f5d9023af3f068733ab68cf81b741f58e3c75ae28d2a9bd07f30b74c38356"))
            (     41300, uint256S("0x8c4e02f6c0d20e856fd7e952a147fee30ce145ca6932a284f354924362d2b408"))
            (   1114311, uint256S("0x93515f222f16a9ff3db6594e5ee7c12924cff9ba05b01dbe551d0a9e65dd141f"))
            (   1593400, uint256S("0xe97230c788e7240eb325576810fee62f5162905f63a832f15928b88ac6a938c6"))
            (   1776411, uint256S("0x4f6de194bd2f4580e2ac9337289c7cca348d3f35c9079af2760b288315b82feb")),
            1498636263, // * UNIX timestamp of last checkpoint block
            3660284,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            6000.0     // * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 2100000;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.BIP34Height = 600;
        consensus.BIP34Hash = uint256S("0x03d3e9191e34ca47e86b1dd8d22213f20c6058bd2533eea04ab7d3655eb13646");
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powNeoScryptLimit = uint256S("0000003fffff0000000000000000000000000000000000000000000000000000");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60; // 2.5 mins
        consensus.checkpointPubKey = "0421c27bb6580b05dcda1f47e59274489f094a3e85d96bbc38d5befd10eee97397ec8a93b6d8d79e8370239a8f39adf66322b41dafe83066bbcee6144e4c41a699";
        consensus.vAlertPubKey = ParseHex("04ee30d11e8de34c8c40410d7aefed4865e9d9978335239dd4869e62651030d9a18332537c03ff24580fe668cfcdf087341715b56b1c0788b600631ed4445d3280");
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 375; // 75% of 500 block
        consensus.nMinerConfirmationWindow = 500;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1514764800; // Jan 1st, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1546300800; // Jan 1st, 2019

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1514764800; // Jan 1st, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1546300800; // Jan 1st, 2019

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000001000");

        pchMessageStart[0] = 0xda;
        pchMessageStart[1] = 0xaf;
        pchMessageStart[2] = 0xa5;
        pchMessageStart[3] = 0xba;
        pchMessageStartNew[0] = 0x91;
        pchMessageStartNew[1] = 0x65;
        pchMessageStartNew[2] = 0x6a;
        pchMessageStartNew[3] = 0x71;
        nDefaultPort = 19336;
        nPruneAfterHeight = 1000;

        consensus.nForkTwo = 0;
        consensus.nForkThree = 0;
        consensus.nForkFour = 0;
        consensus.nTimeLimit = 100;
        consensus.nVersionBitSwitch = 400;
        consensus.nNeoScryptSwitch = 1413936000;
        consensus.nNeoScryptFork = 1486758327;

        genesis = CreateGenesisBlock(1396255061, 677449, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x7734b3734ab1f0d0758e6c274622a377092549df05f6a4fe6939cbc754939169"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("testnet-explorer2.feathercoin.com","feathercoin.com", false));
        vSeeds.push_back(CDNSSeedData("testnet-dnsseed.feathercoin.com","feathercoin.com", false));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;


        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("8e8b634d2f2800398261b7adcfbb6ace490e1746e62123ec2bf8010f9fc98b17")),
            1396255061,
            1,
            300
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 256;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = -1; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powNeoScryptLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60; // 2.5 mins
        consensus.checkpointPubKey = "04db371d2b99df9e7962b2ce2369ada9821261e7f943a57a7ff3494d7d6c87939d3632fcaa2878bd5e45cfea0ea435fd6ee049a28ccbd0998ea3aadeb7abeed065";
        consensus.vAlertPubKey = ParseHex("04e7b36458cb1db28567a99391109bc55a0c55623836d93d8794db6549dcc590012d1f5e23c786b752650dadce34fe5504dd7332450392eeb8292e62b211920c78");
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        pchMessageStart[0] = 0xda;
        pchMessageStart[1] = 0xaf;
        pchMessageStart[2] = 0xa5;
        pchMessageStart[3] = 0xba;
        pchMessageStartNew[0] = 0xd1;
        pchMessageStartNew[1] = 0xa5;
        pchMessageStartNew[2] = 0xaa;
        pchMessageStartNew[3] = 0xb1;
        nDefaultPort = 18446;
        nPruneAfterHeight = 1000;

        consensus.nForkTwo = 0;
        consensus.nForkThree = 0;
        consensus.nForkFour = 0;
        consensus.nVersionBitSwitch = 400;
        consensus.nNeoScryptSwitch = 1524127760;
        consensus.nNeoScryptFork = 1524127760;

        genesis = CreateGenesisBlock(1515840634, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xc797e583cd93b97b1370a687519606abebd2b84fc4294b0e7584f59187e23ebb"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("c797e583cd93b97b1370a687519606abebd2b84fc4294b0e7584f59187e23ebb")),
            0,
            0,
            0
        };
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}

