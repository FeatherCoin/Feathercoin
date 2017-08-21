// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015-2017 The Feathercoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"
#include "main.h"

// Feathercoin:
int nTargetTimespan; 
int nTargetSpacing;

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    const CChainParams& chainParams = Params();

    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();
    unsigned int nProofOfWorkLimitNeo = UintToArith256(params.powNeoLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // The next block
    int nHeight = pindexLast->nHeight + 1;
    // if (Debug) LogPrintf("GetNextWorkRequired.1 pindexLast block Height=%d,nBits=%d \n",pindexLast->nHeight,pindexLast->nBits);

    /* The 4th hard fork and testnet hard fork */
    if ((nHeight >= nForkFour)|| ((chainParams.NetworkIDString()=="test") && (nHeight >= nTestnetFork))) {
        if(!fNeoScrypt) fNeoScrypt = true;
        /* Difficulty reset after the switch */
        if((nHeight == nForkFour)|| ((chainParams.NetworkIDString()=="test") && (nHeight == nTestnetFork)))
        {
          // if (fDebug) LogPrintf("GetNextWorkRequired.1.00 4th hard fork\n");
          return nProofOfWorkLimitNeo;
         }
    }
    
    nTargetTimespan=params.nPowTargetTimespan;
    nTargetSpacing=params.nPowTargetSpacing;
    
    if (nHeight >= nForkOne)
				nTargetTimespan = (7 * 24 * 60 * 60) / 8; // 7/8 days

    if (nHeight >= nForkTwo)
				nTargetTimespan = (7 * 24 * 60 * 60) / 32; // 7/32 days
		
    if ((nHeight >= nForkThree) || (chainParams.NetworkIDString()=="test")) {
        nTargetTimespan = 60; // 1 minute timespan
        nTargetSpacing = 60; // 1 minute block
    }
    
    // 2016 blocks initial, 504 after the 1st, 126 after the 2nd hard fork, 15 after the 3rd hard fork
    //int nInterval = params.DifficultyAdjustmentInterval();
    int nInterval = nTargetTimespan / nTargetSpacing;

    bool fHardFork = (nHeight == nForkOne) || (nHeight == nForkTwo) || (nHeight == nForkThree) || (nHeight == nForkFour);
    if(chainParams.NetworkIDString()=="test") {
        if (nHeight == nTestnetFork) {
            fHardFork = true;
        } else {
            fHardFork = false;
        }
    }

	// if (Debug) LogPrintf("GetNextWorkRequired.1.1 nInterval=%d,nTargetTimespan=%d,nTargetSpacing=%d \n",nInterval,nTargetTimespan,nTargetSpacing);    
    // Only change once per difficulty adjustment interval
    // Difficulty rules regular blocks
    // if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    if((nHeight % nInterval != 0) && !(fHardFork) && (nHeight < nForkThree))
    {
        if (params.fPowAllowMinDifficultyBlocks && (chainParams.NetworkIDString()=="test"))
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            //if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + nTargetSpacing*2)
            {
                // if (fDebug) LogPrintf("GetNextWorkRequired.1.2 nProofOfWorkLimit=%d\n",nProofOfWorkLimit);
                return nProofOfWorkLimit;
             }
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                // if (fDebug) LogPrintf("main,GetNextWorkRequired.1.3 nBits=%d\n",pindex->nBits);
                return pindex->nBits;
            }
        }
        // if (Debug) LogPrintf("GetNextWorkRequired.1.4 nBits=%d\n",pindexLast->nBits);
        return pindexLast->nBits;
    }
    // if (fDebug) LogPrintf("GetNextWorkRequired.2 Difficulty rules regular blocks,nHeight=%d,nInterval=%d,nHeight nInterval=%d\n",nHeight,nInterval,nHeight % nInterval);
    
    
    // The 1st retarget after genesis
    if(nInterval >= nHeight) nInterval = nHeight - 1;

    // Go back by nInterval
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < nInterval; i++)
        pindexFirst = pindexFirst->pprev;
    assert(pindexFirst);

	// if (fDebug) LogPrintf("GetNextWorkRequired.3 Difficulty pindexFirst->nHeight=%d,nInterval=%d,nHeight=%d\n",pindexFirst->nHeight,nInterval,nHeight);
    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params,nInterval,nHeight);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params,int nInterval,int nHeight)
{
		const CChainParams& chainParams = Params();
		
    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    //if (fDebug) LogPrintf("CalculateNextWorkRequired.1 nActualTimespan = %d  before bounds\n", nActualTimespan);
    
        
    // Additional averaging over 4x nInterval window
    if((nHeight >= nForkTwo) && (nHeight < nForkThree)) {
        nInterval *= 4;

        const CBlockIndex* pindexFirst = pindexLast;
        for(int i = 0; pindexFirst && i < nInterval; i++)
          pindexFirst = pindexFirst->pprev;

        int nActualTimespanLong =
          (pindexLast->GetBlockTime() - pindexFirst->GetBlockTime())/4;

        // Average between short and long windows
        int nActualTimespanAvg = (nActualTimespan + nActualTimespanLong)/2;

        // Apply .25 damping
        nActualTimespan = nActualTimespanAvg + 3*nTargetTimespan;
        nActualTimespan /= 4;

        // if (fDebug)  LogPrintf("CalculateNextWorkRequired.2 RETARGET: >nForkTwo and <nForkThree nActualTimespanLong = %d, nActualTimeSpanAvg = %d, nActualTimespan (damped) = %d\n", nActualTimespanLong, nActualTimespanAvg, nActualTimespan);
    }
    
  	//eHRC  
		//Additional averaging over 15, 120 and 480 block window
    if((nHeight >= nForkThree) || (chainParams.NetworkIDString()=="test")) 
   	{
        nInterval *= 480;

        int pindexFirstShortTime = 0;
        int pindexFirstMediumTime = 0;
        const CBlockIndex* pindexFirstLong = pindexLast;
				for(int i = 0; pindexFirstLong && i < nInterval && i < nHeight - 1; i++) 
				{  // i < nHeight - 1 special rule for testnet
					pindexFirstLong = pindexFirstLong->pprev;
					if (i == 14) {
				            pindexFirstShortTime = pindexFirstLong->GetBlockTime();
					}
					if (i == 119) {
				            pindexFirstMediumTime = pindexFirstLong->GetBlockTime();
					}
				}

				int nActualTimespanShort =
				    (pindexLast->GetBlockTime() - pindexFirstShortTime)/15;
				
				int nActualTimespanMedium =
				    (pindexLast->GetBlockTime() - pindexFirstMediumTime)/120;
				
				int nActualTimespanLong =
						(pindexLast->GetBlockTime() - pindexFirstLong->GetBlockTime())/480;
				
				int nActualTimespanAvg = 0;
				nActualTimespanAvg = (nActualTimespanShort + nActualTimespanMedium + nActualTimespanLong)/3;
				
				// Apply .25 damping
				nActualTimespan = nActualTimespanAvg + 3*nTargetTimespan;
				nActualTimespan /= 4;
				
			// if (fDebug) LogPrintf("CalculateNextWorkRequired.3 RETARGET: >nForkThree nActualTimespanShort = %d, nActualTimespanMedium = %d, nActualTimespanLong = %d, nActualTimeSpanAvg = %d, nActualTimespan (damped) = %d\n",nActualTimespanShort, nActualTimespanMedium, nActualTimespanLong, nActualTimespanAvg, nActualTimespan);
    }

    // The initial settings (4.0 difficulty limiter)
    int nActualTimespanMax = nTargetTimespan*4;
    int nActualTimespanMin = nTargetTimespan/4;

    // The 1st hard fork (1.4142857 aka 41% difficulty limiter)
    if(nHeight >= nForkOne) {
        nActualTimespanMax = nTargetTimespan*99/70;
        nActualTimespanMin = nTargetTimespan*70/99;
    }

    // The 2nd hard fork (1.0905077 aka 9% difficulty limiter)
    if ((nHeight >= nForkTwo)||(chainParams.NetworkIDString()=="test")) {
        nActualTimespanMax = nTargetTimespan*494/453;
        nActualTimespanMin = nTargetTimespan*453/494;
    }

    if(nActualTimespan < nActualTimespanMin) nActualTimespan = nActualTimespanMin;
    if(nActualTimespan > nActualTimespanMax) nActualTimespan = nActualTimespanMax;

    //if (fDebug) LogPrintf("CalculateNextWorkRequired.4 RETARGET: nActualTimespan = %d after bounds\n", nActualTimespan);
    //if (fDebug) LogPrintf("CalculateNextWorkRequired.4 RETARGET: nTargetTimespan = %d, nTargetTimespan/nActualTimespan = %.4f\n", nTargetTimespan, (float) nTargetTimespan/nActualTimespan);

    arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnPowScyptLimit = UintToArith256(uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
    
    // Retarget
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    arith_uint256 bnOld;    
    bnOld = bnNew;
    // watch
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

	// if (fDebug) LogPrintf("CalculateNextWorkRequired.Retarget: pindexLast=%s\n",pindexLast->ToString());
    // if (fDebug) LogPrintf("CalculateNextWorkRequired.Retarget: bnNew       = %s ,%08x ,%d\n", bnNew.ToString(),bnNew.GetCompact(),bnNew.GetCompact());
    // if (fDebug) LogPrintf("CalculateNextWorkRequired.Retarget: bnPowLimit  = %s ,%08x ,%d\n", bnPowLimit.ToString(),bnPowLimit.GetCompact(),bnPowLimit.GetCompact());
    // if (fDebug) LogPrintf("CalculateNextWorkRequired.Retarget: powNeoLimit = %s ,%08x ,%d\n", UintToArith256(params.powNeoLimit).ToString(),UintToArith256(params.powNeoLimit).GetCompact(),UintToArith256(params.powNeoLimit).GetCompact());
	// if (fDebug) LogPrintf("CalculateNextWorkRequired.Retarget: bnPowScyptLimit  = %s ,%08x ,%d\n", bnPowScyptLimit.ToString(),bnPowScyptLimit.GetCompact(),bnPowScyptLimit.GetCompact());
		
		if (chainParams.NetworkIDString()=="main")
				bnPowLimit = bnPowScyptLimit;
				
    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    // debug print
    // if (fDebug) LogPrintf("CalculateNextWorkRequired.5 RETARGET\n");
    // if (fDebug) LogPrintf("nTargetTimespan = %d    nActualTimespan = %d\n", nTargetTimespan, nActualTimespan);
    // if (fDebug) LogPrintf("Before: %08x  %s\n", pindexLast->nBits, bnOld.ToString());
    // if (fDebug) LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;
    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnPowScyptLimit = UintToArith256(uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
    
    const CChainParams& chainParams = Params();
		if (chainParams.NetworkIDString()=="main")
				bnPowLimit = bnPowScyptLimit;
				
    // Check range
    //if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > bnPowLimit)
        return error("CheckProofOfWork(): nBits below minimum work");

    // Check proof of work matches claimed amount
    // if (hash > bnTarget.getuint256())
    if (UintToArith256(hash) > bnTarget)
    {
    	// if (fDebug) LogPrintf("CheckProofOfWork 100,nBits=%d,hash=%s,bnTarget=%s,UintToArith256(hash)=%s\n",nBits,hash.ToString(),bnTarget.GetHex(),UintToArith256(hash).GetHex());
        return error("CheckProofOfWork(): hash doesn't match nBits");
    }

    return true;
}

bool CheckHeaderProofOfWork(int nHeight,uint256 hash_n,uint256 hash_s, unsigned int nBits, const Consensus::Params& params)
{	
	const CChainParams& chainParams = Params();
	if (chainParams.NetworkIDString()=="main") 
	{
			if (nHeight>=nForkFour)
			{
				if (!CheckProofOfWork(hash_n, nBits,params))
				{
					// if (fDebug) LogPrintf("CheckHeaderProofOfWork(),neoscrypt in mainnet, nHeight=%i \n",nHeight);
				    return error("CheckHeaderProofOfWork() neoscrypt in mainnet: proof of work failed.");	
				}
			}
			else
			{
				if (!CheckProofOfWork(hash_s, nBits,params))
				{
					// if (fDebug) LogPrintf("CheckHeaderProofOfWork(),scrypt in mainnet, nHeight=%i \n",nHeight);
				    return error("CheckHeaderProofOfWork() scrypt in mainnet: proof of work failed.");	
				}
			}
	}
	else
	{
				//Check for -testnet or -regtest 
				if (!CheckProofOfWork(hash_n, nBits,params))
				{
						// if (fDebug) LogPrintf("CheckHeaderProofOfWork(),neoscrypt in testnet,proof of work failed. nHeight=%i \n",nHeight);
						if (!CheckProofOfWork(hash_s, nBits,params))
						{
							// if (fDebug) LogPrintf("CheckHeaderProofOfWork(),scrypt in testnet,proof of work failed. nHeight=%i \n",nHeight);
						    return error("CheckHeaderProofOfWork() scrypt in mainnet: proof of work failed.");	
						}
				}
	}
	return true;
}

arith_uint256 GetBlockProof(const CBlockIndex& block)
{
    arith_uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a arith_uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}

int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from, const CBlockIndex& tip, const Consensus::Params& params)
{
    arith_uint256 r;
    int sign = 1;
    if (to.nChainWork > from.nChainWork) {
        r = to.nChainWork - from.nChainWork;
    } else {
        r = from.nChainWork - to.nChainWork;
        sign = -1;
    }
    //r = r * arith_uint256(params.nPowTargetSpacing) / GetBlockProof(tip);
    r = r * arith_uint256(nTargetSpacing) / GetBlockProof(tip);
    if (r.bits() > 63) {
        return sign * std::numeric_limits<int64_t>::max();
    }
    return sign * r.GetLow64();
}
