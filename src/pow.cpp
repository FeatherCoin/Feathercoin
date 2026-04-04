// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>

namespace {
constexpr unsigned int LEGACY_SCRYPT_COMPAT_PROFILE = 0x3;

void GetRetargetParameters(int nHeight, const Consensus::Params& params, int& nTargetTimespan, int& nTargetSpacing)
{
    nTargetTimespan = params.nPowTargetTimespan;
    nTargetSpacing = params.nPowTargetSpacing;

    if (nHeight >= params.nForkOne) {
        nTargetTimespan = (7 * 24 * 60 * 60) / 8; // 7/8 days
    }

    if (nHeight >= params.nForkTwo) {
        nTargetTimespan = (7 * 24 * 60 * 60) / 32; // 7/32 days
    }

    if (nHeight >= params.nForkThree) {
        nTargetTimespan = 60; // 1 minute timespan
        nTargetSpacing = 60; // 1 minute block
    }
}

unsigned int CalculateNextWorkRequiredInternal(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, int nTargetTimespan, int nTargetSpacing, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting) {
        return pindexLast->nBits;
    }

    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    int nHeight = pindexLast->nHeight + 1;
    int64_t nInterval = nTargetTimespan / nTargetSpacing;
    int64_t nActualTimespanAvg = 0;

    if (nHeight >= params.nForkTwo && nHeight < params.nForkThree) {
        nInterval *= 4;
        const CBlockIndex* pindexFirst = pindexLast;
        for (int i = 0; pindexFirst && i < nInterval; ++i) {
            pindexFirst = pindexFirst->pprev;
        }

        assert(pindexFirst != nullptr);
        int64_t nActualTimespanLong = (pindexLast->GetBlockTime() - pindexFirst->GetBlockTime()) / 4;

        // Average between short and long windows.
        nActualTimespanAvg = (nActualTimespan + nActualTimespanLong) / 2;
    }

    if (nHeight >= params.nForkThree) {
        nInterval = 480;

        int64_t pindexFirstShortTime = 0;
        int64_t pindexFirstMediumTime = 0;
        const CBlockIndex* pindexFirstLong = pindexLast;
        for (int i = 0; pindexFirstLong && i < nInterval && i < nHeight - 1; ++i) {
            pindexFirstLong = pindexFirstLong->pprev;
            if (i == 14) {
                pindexFirstShortTime = pindexFirstLong->GetBlockTime();
            }
            if (i == 119) {
                pindexFirstMediumTime = pindexFirstLong->GetBlockTime();
            }
        }

        assert(pindexFirstLong != nullptr);
        int64_t nActualTimespanShort = (pindexLast->GetBlockTime() - pindexFirstShortTime) / 15;
        int64_t nActualTimespanMedium = (pindexLast->GetBlockTime() - pindexFirstMediumTime) / 120;
        int64_t nActualTimespanLong = (pindexLast->GetBlockTime() - pindexFirstLong->GetBlockTime()) / 480;

        nActualTimespanAvg = (nActualTimespanShort + nActualTimespanMedium + nActualTimespanLong) / 3;
    }

    if (nHeight >= params.nForkTwo) {
        // Apply .25 damping.
        nActualTimespan = nActualTimespanAvg + 3 * nTargetTimespan;
        nActualTimespan /= 4;
    }

    // The initial settings (4.0 difficulty limiter).
    int64_t nActualTimespanMax = nTargetTimespan * 4;
    int64_t nActualTimespanMin = nTargetTimespan / 4;

    // The 1st hard fork (1.4142857 aka 41% difficulty limiter).
    if (nHeight >= params.nForkOne && nHeight < params.nForkTwo) {
        nActualTimespanMax = nTargetTimespan * 99 / 70;
        nActualTimespanMin = nTargetTimespan * 70 / 99;
    }

    // The 2nd hard fork (1.0905077 aka 9% difficulty limiter).
    if (nHeight >= params.nForkTwo) {
        nActualTimespanMax = nTargetTimespan * 494 / 453;
        nActualTimespanMin = nTargetTimespan * 453 / 494;
    }

    // Limit adjustment step.
    if (nActualTimespan < nActualTimespanMin) {
        nActualTimespan = nActualTimespanMin;
    }
    if (nActualTimespan > nActualTimespanMax) {
        nActualTimespan = nActualTimespanMax;
    }

    // Retarget.
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    bool fShift = bnNew.bits() > bnPowLimit.bits() - 1;
    if (fShift) {
        bnNew >>= 1;
    }
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;
    if (fShift) {
        bnNew <<= 1;
    }

    if (bnNew > bnPowLimit) {
        bnNew = bnPowLimit;
    }

    return bnNew.GetCompact();
}
}

uint256 GetBlockProofOfWorkHash(const CBlockHeader& block, const Consensus::Params& params)
{
    return block.GetPoWHash(LEGACY_SCRYPT_COMPAT_PROFILE);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();
    int nHeight = pindexLast->nHeight + 1;

    // 4th hard fork, reset difficulty.
    if (nHeight == params.nForkFour) {
        return UintToArith256(params.powNeoScryptLimit).GetCompact();
    }

    int nTargetTimespan;
    int nTargetSpacing;
    GetRetargetParameters(nHeight, params, nTargetTimespan, nTargetSpacing);

    int64_t nInterval = nTargetTimespan / nTargetSpacing;
    bool fHardFork = nHeight == params.nForkOne || nHeight == params.nForkTwo;

    // Only change once per difficulty adjustment interval.
    if ((pindexLast->nHeight + 1) % nInterval != 0 && !fHardFork && nHeight < params.nForkThree) {
        return pindexLast->nBits;
    }

    if (params.fPowAllowMinDifficultyBlocks) {
        // Special difficulty rule for testnet-style chains:
        // If the new block's timestamp is more than 10 block intervals later,
        // allow mining of a min-difficulty block.
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 10) {
            return nProofOfWorkLimit;
        }
    }

    // The 1st retarget after genesis.
    if (nInterval >= nHeight) {
        nInterval = nHeight - 1;
    }

    // Go back by nInterval.
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < nInterval; ++i) {
        pindexFirst = pindexFirst->pprev;
    }
    assert(pindexFirst != nullptr);

    return CalculateNextWorkRequiredInternal(pindexLast, pindexFirst->GetBlockTime(), nTargetTimespan, nTargetSpacing, params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    int nTargetTimespan;
    int nTargetSpacing;
    GetRetargetParameters(pindexLast->nHeight + 1, params, nTargetTimespan, nTargetSpacing);
    return CalculateNextWorkRequiredInternal(pindexLast, nFirstBlockTime, nTargetTimespan, nTargetSpacing, params);
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
