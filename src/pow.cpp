// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Copyright (c) 2016-2019 The DeepWebCash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

//TODO: DO AS BLACKCOIN
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nTargetLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
	if (pindexLast == NULL)
		return nTargetLimit;

    if (pindexLast->nHeight < params.nLastPOWBlock)
    	 return pindexLast->nBits;

    if (pindexLast->nHeight == params.nLastPOWBlock)
        return UintToArith256(params.posLimit).GetCompact();

    nTargetLimit = UintToArith256(params.posLimit).GetCompact();

	int height = pindexLast->nHeight+1;
	int interval = params.DifficultyAdjustmentInterval();
    // Only change once per difficulty adjustment interval
    if (height % interval != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nTargetSpacing * 16) {
                LogPrintf("Reset Target in Testnet\n");
                return nTargetLimit;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    int nHeightFirst = pindexLast->nHeight - (interval-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    int limit = 2;
    int nTargetTimespan = params.nTargetTimespan;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    int64_t realActualTimespan = nActualTimespan;
    if (nActualTimespan < nTargetTimespan/limit)
        nActualTimespan = nTargetTimespan/limit;
    if (nActualTimespan > nTargetTimespan*limit)
        nActualTimespan = nTargetTimespan*limit;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.posLimit);
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    LogPrintf("GetNextWorkRequired RETARGET at nHeight = %d\n", pindexLast->nHeight+1);
    LogPrintf("params.nTargetTimespan = %d    nActualTimespan = %d    realActualTimespan = %d\n", nTargetTimespan, nActualTimespan, realActualTimespan);
    LogPrintf("Before: %08x  %s\n", pindexLast->nBits, bnOld.ToString());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
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
