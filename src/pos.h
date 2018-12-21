#ifndef POS_H
#define POS_H

#include "chain.h"
#include "consensus/validation.h"
#include "txdb.h"

// To decrease granularity of timestamp
// Supposed to be 2^n-1
static const int STAKE_TIMESTAMP_MASK = 10;

static const int MAX_COINSATKE_INPUT = 10;
/** Compute the hash modifier for proof-of-stake */
uint256 ComputeStakeModifier(const CBlockIndex* pindexPrev, const uint256& kernel);

bool CheckStakeKernelHash( const CBlockIndex* pindexPrev, unsigned int nBits, const CCoins& txPrev, const COutPoint& prevout, unsigned int nTimeTx);
bool IsConfirmedInNPrevBlocks(const CDiskTxPos& txindex, const CBlockIndex* pindexFrom, int nMaxDepth, int& nActualDepth);
bool CheckProofOfStake(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, CValidationState &state);
bool VerifySignature(const CMutableTransaction& txFrom, const CTransaction& txTo, unsigned int nIn, unsigned int flags, int nHashType);

#endif // POS_H
