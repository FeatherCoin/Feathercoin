// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <consensus/merkle.h>

#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>

#include <assert.h>
#include <memory>

#include <chainparamsseeds.h>

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
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
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

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
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
        consensus.BIP34Height = 432000;
        consensus.BIP34Hash = uint256S("0x966e30dd04d09232f6f690a04664cd3258abe43eeda2f2291d93706aa494aa54");
        consensus.BIP65Height = 2120750;
        consensus.BIP66Height = 2120750;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powNeoScryptLimit = uint256S("0000003fffff0000000000000000000000000000000000000000000000000000");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60;
        consensus.checkpointPubKey = "0425165000270c45a2df329bb6cc179782e1ddcdd9afc6240b5f465d49b8f480635c43e6b194e98f69e9827fa7a62d4791ee96c9fa90ec11e45af5d6b2dbb76774";
        consensus.vAlertPubKey = ParseHex("043c19a29fe8f763369aea68107e82854af7b072fc7d2d2adb87d2a3b40b51ab0d0e77805096e255a87388b175fd4a49d93d9b6c878004975e41222a3b85086eef");
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 15120; // 75% of 20160
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
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000005383a3c6486afe6");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0xbb4955cff435b39df717aeb144d60ecc2c47b8a133ca7663df66bace6dea97c6"); // 2124200

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x41;
        pchMessageStart[1] = 0x15;
        pchMessageStart[2] = 0x1a;
        pchMessageStart[3] = 0x21;

        // End of the bridge from old to new pchMessageStart
        pchMessageStartOld[0] = 0xfb;
        pchMessageStartOld[1] = 0xc0;
        pchMessageStartOld[2] = 0xb6;
        pchMessageStartOld[3] = 0xdb;

        nDefaultPort = 9336;
        nPruneAfterHeight = 100000;

        consensus.nForkOne = 33000;
        consensus.nForkTwo = 87948;
        consensus.nForkThree = 204639;
        consensus.nForkFour = 432000;
        consensus.nTimeLimit = 2313000;
        consensus.nNeoScryptFork = 1414346265;

        genesis = CreateGenesisBlock(1317972665, 2084524493, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x12a765e31ffd4059bada1e25190f6e98c99d9714d334efa41a195a7e7e04bfe2"));
        assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they dont support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("dnsseed.feathercoin.com");
        vSeeds.emplace_back("dnsseed1.feathercoin.com");
        vSeeds.emplace_back("dnsseed.alltheco.in");
        vSeeds.emplace_back("dnsseed.bushstar.co.uk");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,14);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,142);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xBC, 0x26};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xDA, 0xEE};

        bech32_hrp = "fc";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = {
            {
                {  22267, uint256S("0x23dc7d871fc2a9b994112e978019f6370bab0b8979f557afe77a7ab620224b70")},
                {  31846, uint256S("0xba7d5c0e6d46f6448253290ce037e13975c13ca9c375ae854b6b2f85044fc0f9")},
                {  41300, uint256S("0x8c4e02f6c0d20e856fd7e952a147fee30ce145ca6932a284f354924362d2b408")},
                { 500000, uint256S("0x2b7ea20e3899deb9591015b0a5a589b9f6032ab82e018014fafe11637b1a2daf")},
                {1000000, uint256S("0xb9e03dffe6b43cac38191d1bbb0d74fec21223e0de052928c96f498ba305f918")},
                {1593400, uint256S("0xe97230c788e7240eb325576810fee62f5162905f63a832f15928b88ac6a938c6")},
                {1776411, uint256S("0x4f6de194bd2f4580e2ac9337289c7cca348d3f35c9079af2760b288315b82feb")},
                {2124270, uint256S("0x4f6de194bd2f4580e2ac9337289c7cca348d3f35c9079af2760b288315b82feb")},
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 0000000000000000002d6cca6761c99b3c2e936f9a0e304b7c7651a993f461de (height 506081).
            1520948476, // * UNIX timestamp of last known number of transactions
            4229159,  // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.03         // * estimated number of transactions per second after that timestamp
        };
    }
};

/**
 * Testnet (v4)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 2100000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("0x1caab189318ac7a857f327ebc08fa9d2e9768f46b9f00cabc9a914a1184a29a2");
        consensus.BIP65Height = 451;
        consensus.BIP66Height = 451;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powNeoScryptLimit = uint256S("0000003fffff0000000000000000000000000000000000000000000000000000");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60;
        consensus.checkpointPubKey = "0421c27bb6580b05dcda1f47e59274489f094a3e85d96bbc38d5befd10eee97397ec8a93b6d8d79e8370239a8f39adf66322b41dafe83066bbcee6144e4c41a699";
        consensus.vAlertPubKey = ParseHex("04ee30d11e8de34c8c40410d7aefed4865e9d9978335239dd4869e62651030d9a18332537c03ff24580fe668cfcdf087341715b56b1c0788b600631ed4445d3280");
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 375; // 75% for testchains
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
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000100010");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x54fe00bf048fffe073889abebe84f11a616053ed1e43c3954a0a7e0e20c76219");

        pchMessageStart[0] = 0x91;
        pchMessageStart[1] = 0x65;
        pchMessageStart[2] = 0x6a;
        pchMessageStart[3] = 0x71;

        pchMessageStartOld[0] = 0xda;
        pchMessageStartOld[1] = 0xaf;
        pchMessageStartOld[2] = 0xa5;
        pchMessageStartOld[3] = 0xba;

        nDefaultPort = 19336;
        nPruneAfterHeight = 1000;

        consensus.nForkTwo = 0;
        consensus.nForkThree = 0;
        consensus.nForkFour = 0;
        consensus.nTimeLimit = 100;
        consensus.nNeoScryptFork = 1486758327;

        genesis = CreateGenesisBlock(1396255061, 677449, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x7734b3734ab1f0d0758e6c274622a377092549df05f6a4fe6939cbc754939169"));
        assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet-explorer2.feathercoin.com");
        vSeeds.emplace_back("testnet-dnsseed.feathercoin.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tf";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;


        checkpointData = {
            {
                {6000, uint256S("488c04227b4bf26dfd9ae3b39f0f4ded29573e96530c7a161d57ff53af0c88d0")},
            }
        };

        chainTxData = ChainTxData{
            1517011548,
            6003,
            0.01
        };

    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 256;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powNeoScryptLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60;
        consensus.checkpointPubKey = "0421c27bb6580b05dcda1f47e59274489f094a3e85d96bbc38d5befd10eee97397ec8a93b6d8d79e8370239a8f39adf66322b41dafe83066bbcee6144e4c41a699";
        consensus.vAlertPubKey = ParseHex("04ee30d11e8de34c8c40410d7aefed4865e9d9978335239dd4869e62651030d9a18332537c03ff24580fe668cfcdf087341715b56b1c0788b600631ed4445d3280");
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xd1;
        pchMessageStart[1] = 0xa5;
        pchMessageStart[2] = 0xaa;
        pchMessageStart[3] = 0xb1;

        pchMessageStartOld[0] = 0xda;
        pchMessageStartOld[1] = 0xaf;
        pchMessageStartOld[2] = 0xa5;
        pchMessageStartOld[3] = 0xba;

        nDefaultPort = 18446;
        nPruneAfterHeight = 1000;

        consensus.nForkTwo = 0;
        consensus.nForkThree = 0;
        consensus.nForkFour = 0;
        consensus.nTimeLimit = 0;
        consensus.nNeoScryptFork = 1524127760;

        genesis = CreateGenesisBlock(1515840634, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xc797e583cd93b97b1370a687519606abebd2b84fc4294b0e7584f59187e23ebb"));
        assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = {
            {
                {0, uint256S("c797e583cd93b97b1370a687519606abebd2b84fc4294b0e7584f59187e23ebb")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "fcrt";
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}
